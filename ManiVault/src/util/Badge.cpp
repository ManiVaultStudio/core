// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Badge.h"

#include "Icon.h"

#include <QDebug>

namespace mv::util {

Badge::Badge(QObject* parent /*= nullptr*/, std::uint32_t number /*= 0*/, const QColor& backgroundColor /*= Qt::red*/, const QColor& foregroundColor /*= Qt::white*/, Qt::Alignment alignment /*= Qt::AlignTop | Qt::AlignRight*/, float scale /*= 0.5f*/) :
    QObject(parent),
    _enabled(false),
    _number(number),
    _backgroundColor(backgroundColor),
    _foregroundColor(foregroundColor),
    _alignment(alignment),
    _scale(scale)
{
}

QPixmap Badge::getPixmap() const
{
    if (!_customPixmap.isNull())
        return _customPixmap;

    return gui::createNumberBadgeOverlayPixmap(_number, _backgroundColor, _foregroundColor);
}

bool Badge::getEnabled() const
{
    return _enabled;
}

void Badge::setEnabled(bool enabled)
{
    if (enabled == _enabled)
        return;

    _enabled = enabled;

    emit enabledChanged(_enabled);
    emit changed();
}

std::uint32_t Badge::getNumber() const
{
    return _number;
}

void Badge::setNumber(std::uint32_t number)
{
    if (number == _number)
        return;

    _number = number;

    emit numberChanged(_number);
    emit changed();
}

QColor Badge::getBackgroundColor() const
{
    return _backgroundColor;
}

void Badge::setBackgroundColor(const QColor& backgroundColor)
{
    if (backgroundColor == _backgroundColor)
        return;

    _backgroundColor = backgroundColor;

    emit backgroundColorChanged(_backgroundColor);
    emit changed();
}

QColor Badge::getForegroundColor() const
{
    return _foregroundColor;
}

void Badge::setForegroundColor(const QColor& foregroundColor)
{
    if (foregroundColor == _foregroundColor)
        return;

    _foregroundColor = foregroundColor;

    emit foregroundColorChanged(_foregroundColor);
    emit changed();
}

Qt::Alignment Badge::getAlignment() const
{
    return _alignment;
}

void Badge::setAlignment(Qt::Alignment alignment)
{
    if (alignment == _alignment)
        return;

    _alignment = alignment;

    emit alignmentChanged(_alignment);
    emit changed();
}

float Badge::getScale() const
{
    return _scale;
}

void Badge::setScale(float scale)
{
    if (scale == _scale)
        return;

    _scale = scale;

    emit scaleChanged(_scale);
    emit changed();
}

QPixmap Badge::getCustomPixmap() const
{
    return _customPixmap;
}

void Badge::setCustomPixmap(const QPixmap& customPixmap)
{
    _customPixmap = customPixmap;

    emit customPixmapChanged(_customPixmap);
    emit changed();
}

}
