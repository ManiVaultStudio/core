// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "IconFont.h"
#include "Icon.h"

#include <stdexcept>

#include <QFontDatabase>
#include <QDebug>
#include <QMessageBox>
#include <QPainter>
#include <QApplication>

using namespace hdps::gui;

namespace hdps {

IconFont::IconFont(const QString& name, const std::uint32_t& majorVersion, const std::uint32_t& minorVersion) :
    _name(name),
    _majorVersion(majorVersion),
    _minorVersion(minorVersion),
    _fontResourceName(QString(":/IconFonts/%1.otf").arg(getFullName())),
    _fontFamily(),
    _characters()
{
    try
    {
        const auto result = QFontDatabase::addApplicationFont(_fontResourceName);

        if (result < 0) {
            throw std::runtime_error(QString("Unable to load %1").arg(getFullName()).toStdString().c_str());
        }
        else {
            qDebug() << "Loaded" << getFullName() << QFontDatabase::applicationFontFamilies(result);

            _fontFamily = QFontDatabase::applicationFontFamilies(result).first();
        }
    }
    catch (std::exception& e)
    {
        QMessageBox::critical(nullptr, "An icon font error occurred", e.what());
    }
    catch (...) {
        QMessageBox::critical(nullptr, "An icon font error occurred", "Unable to load icon font due to an unhandled exception");
    }
}

QString IconFont::getName() const
{
    return _name;
}

QString IconFont::getFullName() const
{
    return QString("%1-%2").arg(_name, getVersionString());
}

std::uint32_t IconFont::getMajorVersion() const
{
    return _majorVersion;
}

std::uint32_t IconFont::getMinorVersion() const
{
    return _minorVersion;
}

QString IconFont::getVersionString() const
{
    return QString("%1.%2").arg(QString::number(_majorVersion), QString::number(_minorVersion));
}

QString IconFont::getSearchVersionString(const std::int32_t& majorVersion /*= -1*/, const std::int32_t& minorVersion /*= -1*/)
{
    const auto major = majorVersion < 0 ? "x" : QString::number(majorVersion);
    const auto minor = minorVersion < 0 ? "x" : QString::number(minorVersion);

    return QString("%2.%3").arg(major, minor);
}

QFont IconFont::getFont(const int& pointSize /*= -1*/) const
{
    return QFont(_fontFamily, pointSize);
}

QIcon IconFont::getIcon(const QString& name, const QColor& foregroundColor/*= QColor(0, 0, 0, 0)*/, const QColor& backgroundColor/*= Qt::transparent*/) const
{
    try
    {
        QSize pixmapSize(64, 64);

        QPixmap pixmap(pixmapSize);

        pixmap.fill(backgroundColor);

        const auto iconRectangle    = QRect(0, 0, pixmapSize.width(), pixmapSize.height());
        const auto iconText         = getIconCharacter(name);
        
        QColor fontColor = foregroundColor;
        if( fontColor == QColor(0, 0, 0, 0) ) // assume that 0 opacity means we want the system default
        {
            if(qApp->palette().window().color().lightnessF() < 0.5)
            {
                fontColor = QColor(255, 255, 255, 255);
            } else {
                fontColor = QColor(0, 0, 0, 255);
            }
        }

        QPainter painter(&pixmap);

        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
        painter.setRenderHint(QPainter::LosslessImageRendering, true);
        painter.setPen(fontColor);
        painter.setFont(getFont(100.0));

        QFontMetrics fontMetrics(painter.font());

        const auto boundingRectangle = fontMetrics.boundingRect(iconText);

        QRect windowRectangle = boundingRectangle;

        if (windowRectangle.height() > windowRectangle.width())
            windowRectangle.setWidth(windowRectangle.height());

        if (windowRectangle.width() > windowRectangle.height())
            windowRectangle.setHeight(windowRectangle.width());

        const auto margin = 2;

        windowRectangle = windowRectangle.marginsAdded(QMargins(margin, margin, margin, margin));

        painter.setWindow(windowRectangle);
        painter.drawText(windowRectangle, Qt::AlignCenter, iconText);

        return createIcon(pixmap);
    }
    catch (std::exception& e)
    {
        QMessageBox::critical(nullptr, "Unable to retrieve icon", e.what());
        return QIcon();
    }
}

void IconFont::initialize()
{
    qDebug() << QString("Initializing %1").arg(getFullName());

    _characters.clear();
}

}
