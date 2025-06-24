// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StyledIcon.h"
#include "Serializable.h"

#include <QDebug>
#include <QFontDatabase>
#include <QJsonObject>
#include <QCryptographicHash>
#include <QByteArray>
#include <QStyleHints>
#include <QDirIterator>
#include <QStringList>
#include <QPainter>

#ifdef _DEBUG
	#define STYLED_ICON_VERBOSE
#endif

namespace mv::util
{

QStringList findResourcesWithPrefix(const QString& prefix) {
    QStringList matchedResources;

    QDirIterator it(":", QDir::Files, QDirIterator::Subdirectories);

    while (it.hasNext()) {
        QString resourcePath = it.next();
        if (resourcePath.startsWith(prefix)) {
            matchedResources.append(resourcePath);
        }
    }

    return matchedResources;
}

QMap<QString, QVariantMap>          StyledIcon::fontMetadata                = {};
QMap<QString, QFont>                StyledIcon::fonts                       = {};

#ifdef Q_OS_MAC
QString                             StyledIcon::defaultIconFontName         = "FontAwesomeSolid";
#else
QString                             StyledIcon::defaultIconFontName         = "FontAwesomeSolid";
#endif

Version                             StyledIcon::defaultIconFontVersion      = { 6, 7, 2 };
QMap<QString, QPixmap>              StyledIcon::pixmaps                     = {};
QVector<QStringList>                StyledIcon::iconFontPreferenceGroups    = { { "FontAwesomeRegular", "FontAwesomeSolid", "FontAwesomeBrandsRegular" } };
QMap<QString, QVector<Version>>     StyledIcon::iconFontVersions            = {};

StyledIcon::StyledIcon(const QString& iconName /*= ""*/, const QString& iconFontName /*= defaultIconFontName*/, const Version& iconFontVersion /*= defaultIconFontVersion*/)
{
    if (!iconName.isEmpty() && !iconFontName.isEmpty())
		set(iconName, iconFontName, iconFontVersion);
}

StyledIcon::StyledIcon(const QIcon& other)
{
    if (!other.isNull()) {
        _iconSettings._sha = Serializable::createId();

        pixmaps[_iconSettings._sha] = other.pixmap(64, 64);
    }
}

void StyledIcon::set(const QString& iconName, const QString& iconFontName, const util::Version& iconFontVersion)
{
    try
    {
        if (iconName.isEmpty() || iconFontName.isEmpty()) {
            return;
        }

	    _iconName           = iconName;
	    _iconFontName       = iconFontName;
	    _iconFontVersion    = iconFontVersion;

        _iconSettings._sha = generateSha(_iconName, _iconFontName, _iconFontVersion);

        const auto iconFontResourcePath = getIconFontResourcePath(_iconFontName, _iconFontVersion);

	    if (!QFile::exists(iconFontResourcePath))
            throw std::runtime_error(QString("Font resource not found: %1").arg(iconFontResourcePath).toStdString());

	    updateIconPixmap();
	}
	catch (std::exception& e)
	{
        qWarning() << "Unable to set styled icon: " << e.what();
	}
}

StyledIcon StyledIcon::fromFontAwesomeRegular(const QString& iconName, const Version& version /*= defaultIconFontVersion*/)
{
    return StyledIcon(iconName, "FontAwesomeRegular", version);
}

StyledIcon StyledIcon::fromFontAwesomeBrandsRegular(const QString& iconName, const Version& version /*= defaultIconFontVersion*/)
{
    return StyledIcon(iconName, "FontAwesomeBrandsRegular", version);
}

StyledIcon StyledIcon::fromQIcon(const QIcon& icon, const StyledIconMode& mode /*= StyledIconMode::ThemeAware*/)
{
    return StyledIcon(icon).withMode(mode);
}

void StyledIcon::initializeIconFont(const QString& iconFontName, const Version& iconFontVersion)
{
    try
    {
        updateIconFontVersions(iconFontName);

        if (fontMetadata.contains(iconFontName))
            return;

	    const auto iconFontResourceName         = getIconFontResourceName(iconFontName, iconFontVersion);
	    const auto iconFontResourcePath         = getIconFontResourcePath(iconFontName, iconFontVersion);
	    const auto iconFontMetadataResourcePath = getIconFontMetadataResourcePath(iconFontName, iconFontVersion);
	    const auto addApplicationFontResult     = QFontDatabase::addApplicationFont(iconFontResourcePath);

	    if (addApplicationFontResult < 0) {
	        throw std::runtime_error(QString("Unable to load %1").arg(iconFontResourceName).toStdString());
	    }

#ifdef STYLED_ICON_VERBOSE
	    qDebug() << "Loaded" << iconFontResourceName << QFontDatabase::applicationFontFamilies(addApplicationFontResult);
#endif

	    fonts[iconFontResourceName] = QFont(QFontDatabase::applicationFontFamilies(addApplicationFontResult).first(), 100.0);

	    QFile iconFontMetaDataFile;

	    iconFontMetaDataFile.setFileName(iconFontMetadataResourcePath);
	    iconFontMetaDataFile.open(QIODevice::ReadOnly | QIODevice::Text);

	    QString iconFontMetaData = iconFontMetaDataFile.readAll();

	    iconFontMetaDataFile.close();

	    const auto jsonDocument     = QJsonDocument::fromJson(iconFontMetaData.toUtf8());
	    const auto metadataObject   = jsonDocument.object();

	    fontMetadata[iconFontResourceName] = metadataObject.toVariantMap();

        if (!iconFontVersions.contains(iconFontName))
            iconFontVersions[iconFontName] = {};

#ifdef STYLED_ICON_VERBOSE
	    qDebug() << QString("Found %1 %2 icons").arg(QString::number(fontMetadata[iconFontResourceName].count()), iconFontName);
#endif
    }
    catch (std::exception& e)
    {
        qWarning() <<  QString("Unable to initialize styled icon: %1-%2").arg(iconFontName, getIconFontVersionString(iconFontVersion)) << e.what();
    }
}

QPixmap StyledIcon::createIconPixmap(const QString& iconName, const QString& iconFontName, const Version& iconFontVersion, const QColor& foregroundColor/*= QColor(0, 0, 0, 0)*/, const QColor& backgroundColor/*= Qt::transparent*/)
{
    if (iconName.isEmpty() || iconFontName.isEmpty()) {
        qDebug() << "StyledIcon::createIconPixmap()" << iconName << iconFontName;
        return {};
    }
    
    try
    {
        const auto iconFontResourceName = getIconFontResourceName(iconFontName, iconFontVersion);

        QSize pixmapSize(64, 64);

        QPixmap pixmap(pixmapSize);

        pixmap.fill(backgroundColor);

        const auto iconText = getIconCharacter(iconName, iconFontName, iconFontVersion);

        QPainter painter(&pixmap);

        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
        painter.setRenderHint(QPainter::LosslessImageRendering, true);
        painter.setPen(foregroundColor);
        painter.setFont(fonts[iconFontResourceName]);

        QFontMetrics fontMetrics(painter.font());

        const auto boundingRectangle = fontMetrics.boundingRect(iconText);

        QRect windowRectangle = boundingRectangle;

        if (windowRectangle.height() > windowRectangle.width())
            windowRectangle.setWidth(windowRectangle.height());

        if (windowRectangle.width() > windowRectangle.height())
            windowRectangle.setHeight(windowRectangle.width());

        constexpr auto margin = 2;

        windowRectangle = windowRectangle.marginsAdded(QMargins(margin, margin, margin, margin));

        painter.setWindow(windowRectangle);
        painter.drawText(windowRectangle, Qt::AlignCenter, iconText);

        return pixmap;
    }
    catch (std::exception& e)
    {
        qWarning() << "Unable to retrieve icon: " << e.what();
        return {};
    }
}

QString StyledIcon::getIconFontVersionString(const Version& iconFontVersion)
{
    return QString("%1.%2.%3").arg(QString::number(iconFontVersion.getMajor()), QString::number(iconFontVersion.getMinor()), QString::number(iconFontVersion.getPatch()));
}

StyledIcon& StyledIcon::withColorGroups(const QPalette::ColorGroup& colorGroupLightTheme, const QPalette::ColorGroup& colorGroupDarkTheme)
{
    _iconSettings._colorGroupLightTheme = colorGroupLightTheme;
    _iconSettings._colorGroupDarkTheme  = colorGroupDarkTheme;

    return *this;
}

StyledIcon& StyledIcon::withColorRoles(const QPalette::ColorRole& colorRoleLightTheme, const QPalette::ColorRole& colorRoleDarkTheme)
{
    _iconSettings._colorRoleLightTheme  = colorRoleLightTheme;
    _iconSettings._colorRoleDarkTheme   = colorRoleDarkTheme;

    return *this;
}

StyledIcon& StyledIcon::withBadge(const Badge::Parameters& badgeParameters)
{
    _iconSettings._badgeParameters = badgeParameters;

    return *this;
}

StyledIcon& StyledIcon::withColor(const QColor& color)
{
    _iconSettings._mode         = StyledIconMode::FixedColor;
    _iconSettings._fixedColor   = color;

    updateIconPixmap();

    return *this;
}

StyledIcon StyledIcon::withMode(const StyledIconMode& mode)
{
    _iconSettings._mode = mode;

    return *this;
}

StyledIcon StyledIcon::withModifier(const QString& iconName, const QString& iconFontName, const Version& iconFontVersion)
{
    try
    {
        if (iconName.isEmpty() || iconFontName.isEmpty()) {
            return *this;
        }

        _modifierIconName           = iconName;
        _modifierIconFontName       = iconFontName;
        _modifierIconFontVersion    = iconFontVersion;
        _iconSettings._modifierSha  = generateSha(_modifierIconName, _modifierIconFontName, _modifierIconFontVersion);

        const auto iconFontResourcePath = getIconFontResourcePath(_modifierIconFontName, _modifierIconFontVersion);

        if (!QFile::exists(iconFontResourcePath))
            throw std::runtime_error(QString("Font resource not found: %1").arg(iconFontResourcePath).toStdString());

        pixmaps[_iconSettings._modifierSha] = createIconPixmap(_modifierIconName, _modifierIconFontName, _modifierIconFontVersion, _iconSettings._mode == StyledIconMode::FixedColor ? Qt::black : _iconSettings._fixedColor);
    }
    catch (std::exception& e)
    {
        qWarning() << "Unable to set icon modifier: " << e.what();
    }

    return *this;
}

QFont StyledIcon::getIconFont(std::int32_t fontPointSize /*= -1*/, const QString& iconFontName /*= defaultIconFontName*/, const Version& iconFontVersion /*= defaultIconFontVersion*/)
{
    const auto iconFontResourceName = getIconFontResourceName(iconFontName, iconFontVersion);

    if (!fonts.contains(iconFontResourceName))
        initializeIconFont(iconFontName, iconFontVersion);

    if (!fonts.contains(iconFontResourceName))
        throw std::runtime_error(QString("Unable to retrieve icon font: %1").arg(iconFontResourceName).toStdString());

    return { fonts[iconFontResourceName].family(), fontPointSize };
}

QString StyledIcon::getIconCharacter(const QString& iconName, const QString& iconFontName /*= defaultIconFontName*/, const Version& iconFontVersion /*= defaultIconFontVersion*/)
{
    try {
	    if (iconName.isEmpty())
	        throw std::runtime_error("Icon name is empty");

	    if (iconFontName.isEmpty())
	        throw std::runtime_error("Icon font name is empty");

        const auto it = std::find_if(iconFontPreferenceGroups.begin(), iconFontPreferenceGroups.end(), [iconFontName](const QStringList& iconFontPreferenceGroup) {
            return iconFontPreferenceGroup.contains(iconFontName);
		});

        if (it == iconFontPreferenceGroups.end())
            throw std::runtime_error(QString("'%1' not found in icon font preference groups").arg(iconFontName).toStdString());

        auto iconFontVariants = *it;

        // Make sure the requested font name gets precedence over the other
        iconFontVariants.removeOne(iconFontName);
        iconFontVariants.prepend(iconFontName);

        QStringList iconFontResourceNames;

        QVector<QPair<QString, QString>> iconFontCandidates;

        // Build up a list of icon font candidates
        for (const auto& candidateIconFontName : iconFontVariants) {
            updateIconFontVersions(candidateIconFontName);

            for (const auto& candidateIconFontVersion : iconFontVersions[candidateIconFontName])
				iconFontCandidates.append({ candidateIconFontName, QString::fromStdString(candidateIconFontVersion.getVersionString()) });
        }

        // Try all candidates
        for (const auto& iconFontCandidate : iconFontCandidates) {
            const auto candidateIconFontName    = iconFontCandidate.first;
            const auto candidateIconFontVersion = iconFontCandidate.second;
            const auto iconFontResourceName     = getIconFontResourceName(candidateIconFontName, candidateIconFontVersion);

#ifdef STYLED_ICON_VERBOSE
            qDebug() << "Trying" << candidateIconFontName << candidateIconFontVersion;
#endif

            iconFontResourceNames << iconFontResourceName;

            if (!fontMetadata.keys().contains(iconFontResourceName)) {
                initializeIconFont(candidateIconFontName, candidateIconFontVersion);
            }

            if (!fontMetadata[iconFontResourceName].keys().contains(iconName)) {
                continue;
            }

            auto status = false;

            const auto iconMap      = fontMetadata[iconFontResourceName][iconName].toMap();
            const auto codePoint    = iconMap["unicode"].toString().toUInt(&status, 16);

            if (status)
                return QChar(codePoint);
        }

        throw std::runtime_error(QString("%1 not found in any of these: %2").arg(iconName, iconFontVariants.join(", ")).toStdString());
	}
	catch (std::exception& e)
	{
	    qWarning() << "Unable to retrieve icon character: " << e.what();
	}

    return "";
}

Badge::Parameters StyledIcon::getBadge() const
{
    return _iconSettings._badgeParameters;
}

void StyledIcon::setBadgeEnabled(bool badgeEnabled)
{
	_iconSettings._badgeParameters._enabled = badgeEnabled;
}

bool StyledIcon::isBadgeEnabled() const
{
    return _iconSettings._badgeParameters._enabled;
}

QString StyledIcon::getIconFontResourceName(const QString& iconFontName, const Version& iconFontVersion)
{
    return QString("%1-%2").arg(iconFontName, getIconFontVersionString(iconFontVersion));
}

QString StyledIcon::getIconFontResourcePath(const QString& iconFontName, const Version& iconFontVersion)
{
    return QString(":/IconFonts/%1.otf").arg(getIconFontResourceName(iconFontName, iconFontVersion));
}

QString StyledIcon::getIconFontMetadataResourcePath(const QString& iconFontName, const Version& iconFontVersion)
{
    auto iconFontMetadataResourcePath = QString(":/IconFonts/%1.json").arg(getIconFontResourceName(iconFontName, iconFontVersion));

    if (iconFontName.contains("FontAwesome"))
        return iconFontMetadataResourcePath.replace("Brands", "").replace("Regular", "").replace("Solid", "");

    return iconFontMetadataResourcePath;
}

void StyledIcon::updateIconPixmap() const
{
    try {
        if (!_iconSettings._sha.isEmpty() && !pixmaps.contains(_iconSettings._sha)) {
	        pixmaps[_iconSettings._sha] = createIconPixmap(_iconName, _iconFontName, _iconFontVersion, _iconSettings._mode == StyledIconMode::FixedColor ? Qt::black : _iconSettings._fixedColor);
        }
	}
	catch (std::exception& e)
	{
        qWarning() << "Unable to update styled icon" << e.what();
	}
}

QString StyledIcon::generateSha(const QString& iconName, const QString& iconFontName, const Version& iconFontVersion)
{
    const auto input    = iconName + iconFontName + QString::fromStdString(iconFontVersion.getVersionString());
	const auto hash     = QCryptographicHash::hash(input.toUtf8(), QCryptographicHash::Sha256);

	return hash.toHex();
}

void StyledIcon::updateIconFontVersions(const QString& iconFontName)
{
#ifdef STYLED_ICON_VERBOSE
    qDebug() << "Updating" << iconFontName << "versions";
#endif

    if (iconFontVersions.contains(iconFontName))
        return;

    // Get icon fonts with matching icon font name
    const auto matchingResources = findResourcesWithPrefix(QString(":/IconFonts/%1").arg(iconFontName));

    // Extract version string from a matching resource name
    const auto extractVersionString = [](const QString& text) -> QString {
        QRegularExpression versionRegex(R"(\b\d+\.\d+\.\d+\b)");
        QRegularExpressionMatch match = versionRegex.match(text);

        if (match.hasMatch()) {
            return match.captured(0);
        }

        return {};
    };

    iconFontVersions[iconFontName].clear();

    // Build up a list of versions from the matching resources
    for (const auto& matchingResourceName : matchingResources) {
        const auto versionString = extractVersionString(matchingResourceName);

        if (versionString.isEmpty())
            continue;

        iconFontVersions[iconFontName] << versionString;
    }

    // Sort based on extracted version
    std::sort(iconFontVersions[iconFontName].begin(), iconFontVersions[iconFontName].end());
    std::reverse(iconFontVersions[iconFontName].begin(), iconFontVersions[iconFontName].end());

#ifdef STYLED_ICON_VERBOSE
    QStringList versionStrings;

    for (const auto& version : iconFontVersions[iconFontName])
        versionStrings << QString::fromStdString(version.getVersionString());

    qDebug() << iconFontName << "versions: " << versionStrings.join(", ");
#endif
}

}
