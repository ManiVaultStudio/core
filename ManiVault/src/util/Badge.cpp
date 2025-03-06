// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Badge.h"

#include "Icon.h"

#include <QDebug>

namespace mv::util {

Badge::Badge(QObject* parent /*= nullptr*/, std::uint32_t number /*= 0*/, const QColor& backgroundColor /*= Qt::red*/, const QColor& foregroundColor /*= Qt::white*/, Qt::Alignment alignment /*= Qt::AlignTop | Qt::AlignRight*/, float scale /*= 0.5f*/) :
    QObject(parent),
    _parameters(number, backgroundColor, foregroundColor, alignment, scale)
{
}

QPixmap Badge::getPixmap() const
{
    if (!_parameters._customPixmap.isNull())
        return _parameters._customPixmap;

    return gui::createNumberBadgeOverlayPixmap(_parameters._number, _parameters._backgroundColor, _parameters._foregroundColor);
}

Badge::Parameters Badge::getParameters() const
{
    return _parameters;
}

bool Badge::getEnabled() const
{
    return _parameters._enabled;
}

void Badge::setEnabled(bool enabled)
{
    if (enabled == _parameters._enabled)
        return;

    _parameters._enabled = enabled;

    emit enabledChanged(_parameters._enabled);
    emit changed();
}

std::uint32_t Badge::getNumber() const
{
    return _parameters._number;
}

void Badge::setNumber(std::uint32_t number)
{
    if (number == _parameters._number)
        return;

    _parameters._number = number;

    emit numberChanged(_parameters._number);
    emit changed();
}

QColor Badge::getBackgroundColor() const
{
    return _parameters._backgroundColor;
}

void Badge::setBackgroundColor(const QColor& backgroundColor)
{
    if (backgroundColor == _parameters._backgroundColor)
        return;

    _parameters._backgroundColor = backgroundColor;

    emit backgroundColorChanged(_parameters._backgroundColor);
    emit changed();
}

QColor Badge::getForegroundColor() const
{
    return _parameters._foregroundColor;
}

void Badge::setForegroundColor(const QColor& foregroundColor)
{
    if (foregroundColor == _parameters._foregroundColor)
        return;

    _parameters._foregroundColor = foregroundColor;

    emit foregroundColorChanged(_parameters._foregroundColor);
    emit changed();
}

Qt::Alignment Badge::getAlignment() const
{
    return _parameters._alignment;
}

void Badge::setAlignment(Qt::Alignment alignment)
{
    if (alignment == _parameters._alignment)
        return;

    _parameters._alignment = alignment;

    emit alignmentChanged(_parameters._alignment);
    emit changed();
}

float Badge::getScale() const
{
    return _parameters._scale;
}

void Badge::setScale(float scale)
{
    if (scale == _parameters._scale)
        return;

    _parameters._scale = scale;

    emit scaleChanged(_parameters._scale);
    emit changed();
}

QPixmap Badge::getCustomPixmap() const
{
    return _parameters._customPixmap;
}

void Badge::setCustomPixmap(const QPixmap& customPixmap)
{
    _parameters._customPixmap = customPixmap;

    emit customPixmapChanged(_parameters._customPixmap);
    emit changed();
}

}
