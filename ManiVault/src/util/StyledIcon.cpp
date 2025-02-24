// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StyledIcon.h"
#include "StyledIconEngine.h"
#include "Serializable.h"

#include <QDebug>
#include <QFontDatabase>
#include <QJsonObject>
#include <QCryptographicHash>
#include <QByteArray>

#ifdef _DEBUG
	#define STYLED_ICON_VERBOSE
#endif

using namespace mv::gui;

namespace mv::util
{

QMap<QString, QVariantMap>  StyledIcon::fontMetadata             = {};
QMap<QString, QFont>        StyledIcon::fonts                    = {};
QString                     StyledIcon::defaultIconFontName      = "FontAwesomeSolid";
Version                     StyledIcon::defaultIconFontVersion   = { 6, 7, 2 };
QMap<QString, QPixmap>      StyledIcon::pixmaps                  = {};
QVector<QStringList>        StyledIcon::iconFontPreferenceGroups = { { "FontAwesomeSolid", "FontAwesomeRegular", "FontAwesomeBrandsRegular" } };

StyledIcon::StyledIcon(const QString& iconName /*= ""*/, const QString& iconFontName /*= defaultIconFontName*/, const Version& iconFontVersion /*= defaultIconFontVersion*/, QWidget* parent /*= nullptr*/) :
    QIcon(new StyledIconEngine(*this)),
    _fixColor(false)//,
    //_badge(this)
{
    if (!iconName.isEmpty() && !iconFontName.isEmpty())
        set(iconName, iconFontName, iconFontVersion);
}

StyledIcon::StyledIcon(const StyledIcon& other) :
    QIcon(other)
{
    *this = other;
}

StyledIcon::StyledIcon(const QIcon& icon) :
    QIcon(new StyledIconEngine(*this))
{
    if (!icon.isNull()) {
        _iconEngine->_sha = Serializable::createId();

        pixmaps[_iconEngine->_sha] = icon.pixmap(32, 32);
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

        _iconEngine->_sha = generateSha(_iconName, _iconFontName, _iconFontVersion);

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
	    const auto iconFontResourceName         = getIconFontResourceName(iconFontName, iconFontVersion);
	    const auto iconFontResourcePath         = getIconFontResourcePath(iconFontName, iconFontVersion);
	    const auto iconFontMetadataResourcePath = getIconFontMetadataResourcePath(iconFontName, iconFontVersion);
	    const auto addApplicationFontResult     = QFontDatabase::addApplicationFont(iconFontResourcePath);

	    if (addApplicationFontResult < 0) {
	        throw std::runtime_error(QString("Unable to load %1").arg(iconFontResourceName).toStdString());
	    }

#ifdef NAMED_ICON_VERBOSE
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

#ifdef NAMED_ICON_VERBOSE
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
    setColorGroupLightTheme(colorGroupLightTheme);
    setColorGroupDarkTheme(colorGroupDarkTheme);

    return *this;
}

StyledIcon& StyledIcon::withColorRoles(const QPalette::ColorRole& colorRoleLightTheme, const QPalette::ColorRole& colorRoleDarkTheme)
{
    setColorRoleLightTheme(colorRoleLightTheme);
    setColorRoleDarkTheme(colorRoleDarkTheme);

    return *this;
}

StyledIcon& StyledIcon::withBadge(const Badge& badge)
{
    //_badge = badge;

    return *this;
}

StyledIcon& StyledIcon::withColor(const QColor& color)
{
    _fixColor   = true;
    _color      = color;

    updateIconPixmap();

    return *this;
}

StyledIcon& StyledIcon::withMode(const StyledIconMode& mode)
{
    _iconEngine->_mode = mode;

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

        // Go over the icon font variants in the order of preference
        for (const auto& iconFontVariant : iconFontVariants) {
            const auto iconFontResourceName = getIconFontResourceName(iconFontVariant, iconFontVersion);

            iconFontResourceNames << iconFontResourceName;

#ifdef NAMED_ICON_VERBOSE
            if (iconFontVariants.indexOf(iconFontVariant) >= 1)
                qWarning() << "Trying" << iconFontResourceName;
#endif

            if (!fontMetadata.keys().contains(iconFontResourceName)) {
                initializeIconFont(iconFontVariant, iconFontVersion);
            }

            if (!fontMetadata[iconFontResourceName].keys().contains(iconName)) {

#ifdef NAMED_ICON_VERBOSE
                qWarning() << iconName << "not found in preferred icon font" << iconFontResourceName;

                if (iconFontVariant != iconFontVariants.last())
                    qWarning() << "Attempting remaining variants...";
#endif

                continue;
            }

            auto status = false;

            const auto iconMap      = fontMetadata[iconFontResourceName][iconName].toMap();
            const auto codePoint    = iconMap["unicode"].toString().toUInt(&status, 16);

            if (!status)
                throw std::runtime_error("Cannot establish unicode code point");

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

QPalette::ColorRole StyledIcon::getColorRoleLightTheme() const
{
    return _iconEngine->_colorRoleLightTheme;
}

void StyledIcon::setColorRoleLightTheme(const QPalette::ColorRole& colorRoleLightTheme) const
{
    _iconEngine->_colorRoleLightTheme = colorRoleLightTheme;
}

QPalette::ColorRole StyledIcon::getColorRoleDarkTheme() const
{
    return _iconEngine->_colorRoleDarkTheme;
}

void StyledIcon::setColorRoleDarkTheme(const QPalette::ColorRole& colorRoleDarkTheme) const
{
    _iconEngine->_colorRoleDarkTheme = colorRoleDarkTheme;
}

QPalette::ColorGroup StyledIcon::getColorGroupLightTheme() const
{
    return _iconEngine->_colorGroupLightTheme;
}

void StyledIcon::setColorGroupLightTheme(const QPalette::ColorGroup& colorGroupLightTheme) const
{
    _iconEngine->_colorGroupLightTheme = colorGroupLightTheme;
}

QPalette::ColorGroup StyledIcon::getColorGroupDarkTheme() const
{
    return _iconEngine->_colorGroupDarkTheme;
}

void StyledIcon::setColorGroupDarkTheme(const QPalette::ColorGroup& colorGroupDarkTheme) const
{
    _iconEngine->_colorGroupDarkTheme = colorGroupDarkTheme;
}

//Badge& StyledIcon::getBadge()
//{
//    return _badge;
//}
//
//void StyledIcon::setBadgeEnabled(bool badgeEnabled)
//{
//    _badge.setEnabled(badgeEnabled);
//}
//
//bool StyledIcon::isBadgeEnabled() const
//{
//    return _badge.getEnabled();
//}

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
        if (_iconEngine && !_iconEngine->_sha.isEmpty() && !pixmaps.contains(_iconEngine->_sha))
			pixmaps[_iconEngine->_sha] = createIconPixmap(_iconName, _iconFontName, _iconFontVersion, _fixColor ? _color : _iconEngine->_fixedColor);
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

}
