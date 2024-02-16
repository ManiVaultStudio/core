// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WidgetActionCollapsedWidget.h"
#include "WidgetAction.h"

#include <QDebug>
#include <QPainter>

namespace mv::gui {

WidgetActionCollapsedWidget::WidgetActionCollapsedWidget(QWidget* parent, WidgetAction* action) :
    WidgetActionViewWidget(parent, action),
    _layout(),
    _toolButton(action, this),
    _widgetConfigurationFunction()
{
    initialize(action);
}

WidgetActionCollapsedWidget::WidgetActionCollapsedWidget(QWidget* parent, WidgetAction* action, WidgetConfigurationFunction widgetConfigurationFunction) :
    WidgetActionCollapsedWidget(parent, action)
{
    _widgetConfigurationFunction = widgetConfigurationFunction;

    initialize(action);
}

void WidgetActionCollapsedWidget::setAction(WidgetAction* action)
{
    WidgetActionViewWidget::setAction(action);

    if (getAction() != nullptr)
        _toolButton.removeAction(getAction());

    if (getAction() == nullptr)
        return;

    _toolButton.addAction(getAction());

    const auto updateToolButton = [this]() {
        _toolButton.setIcon(getAction()->icon());
        _toolButton.setToolTip(getAction()->toolTip());
    };

    updateToolButton();

    connect(getAction(), &WidgetAction::changed, this, updateToolButton);

    const auto updateToolButtonReadOnly = [this]() {
        _toolButton.setEnabled(getAction()->isEnabled());
    };

    updateToolButtonReadOnly();

    connect(getAction(), &WidgetAction::enabledChanged, this, updateToolButtonReadOnly);

    const auto updateToolButtonVisibility = [this]() {
        _toolButton.setVisible(getAction()->isVisible());
    };

    updateToolButtonVisibility();

    connect(getAction(), &WidgetAction::visibleChanged, this, updateToolButtonVisibility);
}

WidgetConfigurationFunction WidgetActionCollapsedWidget::getWidgetConfigurationFunction()
{
    return _widgetConfigurationFunction;
}

void WidgetActionCollapsedWidget::initialize(WidgetAction* action)
{
    _layout.setContentsMargins(0, 0, 0, 0);

    _toolButton.setPopupMode(QToolButton::InstantPopup);
    _toolButton.setIconSize(QSize(16, 16));
    _toolButton.setStyleSheet("QToolButton::menu-indicator { image: none; }");

    _layout.addWidget(&_toolButton);

    setLayout(&_layout);

    setAction(action);
}

WidgetActionCollapsedWidget::ToolButton::ToolButton(WidgetAction* action, QWidget* parent /*= nullptr*/) :
    QToolButton(parent),
    _action(action)
{
    if (_action)
        connect(&action->getBadge(), &WidgetActionBadge::changed, this, [this]() -> void { update(); });
}

void WidgetActionCollapsedWidget::ToolButton::paintEvent(QPaintEvent* paintEvent)
{
    QToolButton::paintEvent(paintEvent);

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    const auto margin = 5.0f;

    QPointF center;

    center.setY(height() - margin);
    center.setX(width() - margin);

    painter.setPen(QPen(QBrush(isEnabled() ? Qt::black : Qt::gray), 2.5, Qt::SolidLine, Qt::RoundCap));
    painter.setBrush(Qt::NoBrush);
    painter.drawPoint(center);

    if (_action && _action->getBadge().getEnabled()) {
        const auto& badge               = _action->getBadge();
        const auto drawSize             = size();
        const auto iconPixmapRectangle  = QRectF(QPoint(0, 0), drawSize);
        const auto badgePixmap          = createNumberBadgeOverlayPixmap(badge.getNumber(), badge.getBackgroundColor(), badge.getForegroundColor()).scaled(badge.getScale() * drawSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        const auto badgeRectangle       = QRectF(QPoint(0, 0), badgePixmap.size());
        const auto scaledIconPixmapSize = badge.getScale() * drawSize;
        const auto badgeAlignment       = badge.getAlignment();
        const auto badgeScale           = badge.getScale();

        QPoint origin;

        if (badgeAlignment & Qt::AlignTop)
            origin.setY(0);

        if (badgeAlignment & Qt::AlignVCenter)
            origin.setY((0.5 * drawSize.height()) - ((0.5 * badgeScale) * drawSize.height()));

        if (badgeAlignment & Qt::AlignBottom)
            origin.setY((1.0f - badgeScale) * drawSize.height());

        if (badgeAlignment & Qt::AlignLeft)
            origin.setX(0);

        if (badgeAlignment & Qt::AlignHCenter)
            origin.setX((0.5 * drawSize.width()) - ((0.5 * badgeScale) * drawSize.width()));

        if (badgeAlignment & Qt::AlignRight)
            origin.setX((1.0f - badgeScale) * drawSize.width());

        painter.drawPixmap(origin, badgePixmap);
    }
}

}
