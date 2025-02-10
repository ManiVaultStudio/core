// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "NamedIcon.h"
#include "Application.h"
#include "Icon.h"

#include <QDebug>
#include <QFontDatabase>
#include <QJsonObject>

using namespace mv::gui;

namespace mv::util
{

QMap<QString, QVariant> NamedIcon::fontMetadata = {};
QMap<QString, QFont> NamedIcon::fonts = {};

NamedIcon::NamedIcon(const QString& iconName, const QString& iconFontName, const Version& iconFontVersion) :
    _iconName(iconName),
    _iconFontName(iconFontName),
    _iconFontVersion(iconFontVersion)
{
    connect(&_themeWatcher, &ThemeWatcher::paletteChanged, this, &NamedIcon::updateIcon);

    updateIcon();
}

NamedIcon NamedIcon::fromFontAwesome(const QString& iconName, const Version& version /*= { 6, 5 }*/)
{
    return NamedIcon(iconName, "FontAwesome", version);
}

NamedIcon NamedIcon::fromFontAwesomeBrands(const QString& iconName, const Version& version)
{
    return NamedIcon(iconName, "FontAwesomeBrands", version);
}

void NamedIcon::initializeIconFont(const QString& iconFontName, const Version& iconFontVersion)
{
    const auto iconFontResourceName     = getIconFontResourceName(iconFontName, iconFontVersion);
    const auto metadataResource         = QString(":/IconFonts/%1.json").arg(iconFontResourceName);
    const auto addApplicationFontResult = QFontDatabase::addApplicationFont(iconFontResourceName);

    if (addApplicationFontResult < 0) {
        throw std::runtime_error(QString("Unable to load %1").arg(iconFontResourceName).toStdString().c_str());
    } else {
#ifdef NAMED_ICON_VERBOSE
        qDebug() << "Loaded" << iconFontResourceName << QFontDatabase::applicationFontFamilies(result);
#endif

        fonts[iconFontResourceName] = QFont(QFontDatabase::applicationFontFamilies(addApplicationFontResult).first(), 100.0);
    }

    QFile iconFontMetaDataFile;

    iconFontMetaDataFile.setFileName(metadataResource);
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

QString NamedIcon::getIconCharacter(const QString& iconName, const QString& iconFontName, const Version& iconFontVersion)
{
    const auto iconFontResourceName = getIconFontResourceName(iconFontName, iconFontVersion);

    if (!fontMetadata.keys().contains(iconFontResourceName)) {
        initializeIconFont(iconFontName, iconFontVersion);
    }

    auto status = false;

    const auto codePoint = fontMetadata[iconFontResourceName].toMap()["unicode"].toString().toUInt(&status, 16);

    if (!status)
        throw std::runtime_error("Cannot establish unicode code point");

    return QChar(codePoint);
}

QIcon NamedIcon::getIcon(const QString& iconName, const QString& iconFontName, const Version& iconFontVersion, const QColor& foregroundColor/*= QColor(0, 0, 0, 0)*/, const QColor& backgroundColor/*= Qt::transparent*/)
{
    try
    {
        const auto iconFontResourceName = getIconFontResourceName(iconFontName, iconFontVersion);

        QSize pixmapSize(64, 64);

        QPixmap pixmap(pixmapSize);

        pixmap.fill(backgroundColor);

        const auto iconRectangle    = QRect(0, 0, pixmapSize.width(), pixmapSize.height());
        const auto iconText         = getIconCharacter(iconName, iconFontName, iconFontVersion);

        QColor fontColor = foregroundColor;

        if (fontColor == QColor(0, 0, 0, 0))
            fontColor = qApp->palette().text().color();

        QPainter painter(&pixmap);

        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
        painter.setRenderHint(QPainter::LosslessImageRendering, true);
        painter.setPen(fontColor);
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

        return createIcon(pixmap);
    }
    catch (std::exception& e)
    {
        QMessageBox::critical(nullptr, "Unable to retrieve icon", e.what());
        return {};
    }
}

QString NamedIcon::getIconFontVersionString(const Version& version)
{
    return QString("%1.%2").arg(QString::number(version.getMajor()), QString::number(version.getMinor()));
}

QString NamedIcon::getIconFontResourceName(const QString& iconFontName, const Version& version)
{
    return QString("%1-%2").arg(iconFontName, getIconFontVersionString(version)).replace("Brands", "").replace("Regular", "");
}

void NamedIcon::updateIcon()
{
    const auto textColor = qApp->palette().text().color();

    static_cast<QIcon&>(*this) = Application::getIconFont(_iconFontName, _iconFontVersion.getMajor(), _iconFontVersion.getMinor()).getIcon(_iconName, textColor);

	//QIcon::addPixmap(QPixmap());  // Trick to trigger repaint
}

}
