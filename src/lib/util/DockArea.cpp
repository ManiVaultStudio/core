// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DockArea.h"
#include "Icon.h"
#include "StyledIcon.h"

#include <QPainter>

using namespace mv::util;

namespace mv::gui
{

QIcon getDockAreaIcon(DockAreaFlag dockArea)
{
    const auto size = 128;

    QPixmap pixmap(size, size);

    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);

    painter.setWindow(0, 0, 128, 128);

    const auto t1                   = 5;
    const auto t2                   = 20;
    const auto t3                   = 35;
    const auto t4                   = 50;
    const auto rectangleSize        = size - (2 * t3);
    const auto halfRectangleSize    = rectangleSize / 2;
    const auto lineThickness        = 2.5;

    const auto drawRectangle = [&]() -> void {
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(Qt::black, lineThickness));
        painter.drawRect(QRectF(QPointF(t3, t3), QPointF(size - t3, size - t3)));
    };

    const auto drawDockRectangle = [&](const QRectF& rect) -> void {
        painter.setBrush(Qt::black);
        painter.setPen(QPen(Qt::black, lineThickness));
        painter.drawRect(rect);
    };

    switch (static_cast<std::uint32_t>(dockArea))
    {
        case static_cast<std::uint32_t>(DockAreaFlag::None):
            return QIcon();

        case static_cast<std::uint32_t>(DockAreaFlag::Left):
            drawRectangle();
            drawDockRectangle(QRectF(QPointF(t1, t3), QPointF(t2, size - t3)));
            break;

        case static_cast<std::uint32_t>(DockAreaFlag::Right):
            drawRectangle();
            drawDockRectangle(QRectF(QPointF(size - t1, t3), QPointF(size - t2, size - t3)));
            break;

        case static_cast<std::uint32_t>(DockAreaFlag::Top):
            drawRectangle();
            drawDockRectangle(QRectF(QPointF(t3, t1), QPointF(size - t3, t2)));
            break;

        case static_cast<std::uint32_t>(DockAreaFlag::Bottom):
            drawRectangle();
            drawDockRectangle(QRectF(QPointF(t3, size - t1), QPointF(size - t3, size - t2)));
            break;

        case static_cast<std::uint32_t>(DockAreaFlag::Center):
            drawRectangle();
            drawDockRectangle(QRectF(QPointF(t4, t4), QPointF(size - t4, size - t4)));
            break;
    }

    return QIcon(util::StyledIcon(createIcon(pixmap)));
}

}
