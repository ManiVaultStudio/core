// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WidgetActionBadge.h"

#include <QDebug>

namespace mv::gui {

WidgetActionBadge::WidgetActionBadge(QObject* parent, std::uint32_t number /*= 0*/, const QColor& backgroundColor /*= Qt::red*/, const QColor& foregroundColor /*= Qt::white*/, Qt::Alignment alignment /*= Qt::AlignTop | Qt::AlignRight*/, float scale /*= 0.5f*/) :
    QObject(parent),
    _enabled(false),
    _number(number),
    _backgroundColor(backgroundColor),
    _foregroundColor(foregroundColor),
    _alignment(alignment),
    _scale(scale),
    _customPixmap()
{
    //qApp->palette().highlight().color()
}

QPixmap WidgetActionBadge::getPixmap() const
{
    if (!_customPixmap.isNull())
        return _customPixmap;

    return createNumberBadgeOverlayPixmap(_number, _backgroundColor, _foregroundColor);
}

bool WidgetActionBadge::getEnabled() const
{
    return _enabled;
}

void WidgetActionBadge::setEnabled(bool enabled)
{
    if (enabled == _enabled)
        return;

    _enabled = enabled;

    emit enabledChanged(_enabled);
    emit changed();
}

std::uint32_t WidgetActionBadge::getNumber() const
{
    return _number;
}

void WidgetActionBadge::setNumber(std::uint32_t number)
{
    if (number == _number)
        return;

    _number = number;

    emit numberChanged(_number);
    emit changed();
}

QColor WidgetActionBadge::getBackgroundColor() const
{
    return _backgroundColor;
}

void WidgetActionBadge::setBackgroundColor(const QColor& backgroundColor)
{
    if (backgroundColor == _backgroundColor)
        return;

    _backgroundColor = backgroundColor;

    emit backgroundColorChanged(_backgroundColor);
    emit changed();
}

QColor WidgetActionBadge::getForegroundColor() const
{
    return _foregroundColor;
}

void WidgetActionBadge::setForegroundColor(const QColor& foregroundColor)
{
    if (foregroundColor == _foregroundColor)
        return;

    _foregroundColor = foregroundColor;

    emit foregroundColorChanged(_foregroundColor);
    emit changed();
}

Qt::Alignment WidgetActionBadge::getAlignment() const
{
    return _alignment;
}

void WidgetActionBadge::setAlignment(Qt::Alignment alignment)
{
    if (alignment == _alignment)
        return;

    _alignment = alignment;

    emit alignmentChanged(_alignment);
    emit changed();
}

float WidgetActionBadge::getScale() const
{
    return _scale;
}

void WidgetActionBadge::setScale(float scale)
{
    if (scale == _scale)
        return;

    _scale = scale;

    emit scaleChanged(_scale);
    emit changed();
}

QPixmap WidgetActionBadge::getCustomPixmap() const
{
    return _customPixmap;
}

void WidgetActionBadge::setCustomPixmap(const QPixmap& customPixmap)
{
    _customPixmap = customPixmap;

    emit customPixmapChanged(_customPixmap);
    emit changed();
}

}
