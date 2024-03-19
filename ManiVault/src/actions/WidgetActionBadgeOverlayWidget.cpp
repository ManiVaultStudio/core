// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WidgetActionBadgeOverlayWidget.h"

#include <QPainter>

namespace mv::gui {

WidgetActionBadgeOverlayWidget::WidgetActionBadgeOverlayWidget(QWidget* parent, WidgetAction* action) :
    OverlayWidget(parent, 0.0f),
    _action(action)
{
}

void WidgetActionBadgeOverlayWidget::paintEvent(QPaintEvent* paintEvent)
{
    OverlayWidget::paintEvent(paintEvent);

    if (_action && _action->getBadge().getEnabled()) {
        QPainter painter(this);

        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);

        const auto& badge               = _action->getBadge();
        const auto drawSize             = QSize(height(), height());
        const auto badgePixmap          = createNumberBadgeOverlayPixmap(badge.getNumber(), badge.getBackgroundColor(), badge.getForegroundColor()).scaled(badge.getScale() * drawSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        const auto scaledIconPixmapSize = badge.getScale() * drawSize;
        const auto badgeAlignment       = badge.getAlignment();

        QPoint origin;

        if (badgeAlignment & Qt::AlignTop)
            origin.setY(0);

        if (badgeAlignment & Qt::AlignVCenter)
            origin.setY((0.5 * height()) - (0.5 * scaledIconPixmapSize.height()));

        if (badgeAlignment & Qt::AlignBottom)
            origin.setY(height() - scaledIconPixmapSize.height());

        if (badgeAlignment & Qt::AlignLeft)
            origin.setX(0);

        if (badgeAlignment & Qt::AlignHCenter)
            origin.setX((0.5 * width()) - (0.5 * scaledIconPixmapSize.width()));

        if (badgeAlignment & Qt::AlignRight)
            origin.setX(width() - scaledIconPixmapSize.width());

        painter.drawPixmap(origin, badgePixmap);
    }
}

}
