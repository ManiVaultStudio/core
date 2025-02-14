// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StyledIcon.h"
#include "StyledIconEngine.h"

#include "Application.h"
#include "Serializable.h"

#include <QDebug>
#include <QFontDatabase>
#include <QJsonObject>
#include <QCryptographicHash>
#include <QByteArray>

using namespace mv::gui;

namespace mv::util
{

QMap<QString, QVariantMap>  StyledIcon::fontMetadata             = {};
QMap<QString, QFont>        StyledIcon::fonts                    = {};
QString                     StyledIcon::defaultIconFontName      = "FontAwesomeSolid";
Version                     StyledIcon::defaultIconFontVersion   = { 5, 14 };
QMap<QString, QPixmap>      StyledIcon::pixmaps                  = {};

StyledIcon::StyledIcon(const QString& iconName /*= ""*/, const QString& iconFontName /*= defaultIconFontName*/, const Version& iconFontVersion /*= defaultIconFontVersion*/, QWidget* parent /*= nullptr*/) :
    QObject(parent),
    QIcon(new StyledIconEngine(*this))
{
    connect(&mv::theme(), &AbstractThemeManager::themeChanged, this, &StyledIcon::updateIconPixmap);

    if (!iconName.isEmpty() && !iconFontName.isEmpty())
        set(iconName, iconFontName, iconFontVersion);
}

StyledIcon::StyledIcon(const StyledIcon& other) :
    StyledIcon(other._iconName, other._iconFontName, other._iconFontVersion)
{
}

StyledIcon::StyledIcon(const QIcon& icon) :
    StyledIcon()
{
    if (!icon.isNull()) {
        _iconEngine->_sha = Serializable::createId();

        pixmaps[_iconEngine->_sha] = icon.pixmap(64, 64);
    }
}

void StyledIcon::set(const QString& iconName, const QString& iconFontName, const util::Version& iconFontVersion)
{
    try
    {
        if (iconName.isEmpty() || iconFontName.isEmpty())
            return;

	    const auto iconNameHasChanged          = iconName != _iconName;
	    const auto iconFontNameHasChanged      = iconFontName != _iconFontName;
	    const auto iconFontVersionHasChanged   = iconFontVersion != _iconFontVersion;

	    if (!iconNameHasChanged && !iconFontNameHasChanged && !iconFontVersionHasChanged)
	        return;

	    const auto previousIconName         = _iconName;
	    const auto previousIconFontName     = _iconFontName;
	    const auto previousIconFontVersion  = _iconFontVersion;

	    _iconName           = iconName;
	    _iconFontName       = iconFontName;
	    _iconFontVersion    = iconFontVersion;

        _iconEngine->_sha = generateSha(_iconName, _iconFontName, _iconFontVersion);

        const auto iconFontResourcePath = getIconFontResourcePath(_iconFontName, _iconFontVersion);

	    if (!QFile::exists(iconFontResourcePath))
            throw std::runtime_error(QString("Font resource not found: %1").arg(iconFontResourcePath).toStdString());

	    if (iconNameHasChanged)
			emit iconNameChanged(previousIconName, _iconName);

	    if (iconFontNameHasChanged)
	        emit iconFontNameChanged(previousIconFontName, _iconFontName);

	    if (iconFontVersionHasChanged)
	        emit iconFontVersionChanged(previousIconFontVersion, _iconFontVersion);

	    if (iconNameHasChanged || iconFontNameHasChanged || iconFontVersionHasChanged)
			emit changed();

	    updateIconPixmap();
	}
	catch (std::exception& e)
	{
        qWarning() << "Unable to set styled icon" << e.what();
	}
}

StyledIcon StyledIcon::fromFontAwesomeRegular(const QString& iconName, const Version& version /*= { 6, 5 }*/)
{
    return StyledIcon(iconName, "FontAwesomeRegular", version);
}

StyledIcon StyledIcon::fromFontAwesomeBrandsRegular(const QString& iconName, const Version& version)
{
    return StyledIcon(iconName, "FontAwesomeBrandsRegular", version);
}

QPixmap StyledIcon::requestPixmap() const
{
    if (!pixmaps.contains(_iconEngine->_sha)) {
        pixmaps[_iconEngine->_sha] = createIconPixmap(_iconName, _iconFontName, _iconFontVersion, qApp->palette().text().color());
    }

    return pixmaps[_iconEngine->_sha];
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
	    } else {
#ifdef NAMED_ICON_VERBOSE
	        qDebug() << "Loaded" << iconFontResourceName << QFontDatabase::applicationFontFamilies(result);
#endif

	        fonts[iconFontResourceName] = QFont(QFontDatabase::applicationFontFamilies(addApplicationFontResult).first(), 100.0);
	    }

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

QString StyledIcon::getIconCharacter(const QString& iconName, const QString& iconFontName, const Version& iconFontVersion)
{
    if (iconName.isEmpty())
        throw std::runtime_error("Icon name is empty");

    if (iconFontName.isEmpty())
        throw std::runtime_error("Icon font name is empty");

    const auto iconFontResourceName = getIconFontResourceName(iconFontName, iconFontVersion);

    if (!fontMetadata.keys().contains(iconFontResourceName)) {
        initializeIconFont(iconFontName, iconFontVersion);
    }

    if (!fontMetadata[iconFontResourceName].keys().contains(iconName))
        throw std::runtime_error(QString("'%1' not found in %2").arg(iconName, iconFontResourceName).toStdString());

    auto status = false;

    const auto iconMap      = fontMetadata[iconFontResourceName][iconName].toMap();
    const auto codePoint    = iconMap["unicode"].toString().toUInt(&status, 16);

    if (!status)
        throw std::runtime_error("Cannot establish unicode code point");

    return QChar(codePoint);
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
    return QString("%1.%2").arg(QString::number(iconFontVersion.getMajor()), QString::number(iconFontVersion.getMinor()));
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
        if (pixmaps.contains(_iconEngine->_sha))
            return;

        pixmaps[_iconEngine->_sha] = createIconPixmap(_iconName, _iconFontName, _iconFontVersion, qApp->palette().text().color());
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
