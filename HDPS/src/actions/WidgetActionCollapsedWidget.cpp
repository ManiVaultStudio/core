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

    _toolButton.setObjectName("ToolButton");
    _toolButton.setPopupMode(QToolButton::InstantPopup);
    _toolButton.setIconSize(QSize(16, 16));
    _toolButton.setStyleSheet("QToolButton::menu-indicator { image: none; }");
    _toolButton.setAutoRaise(action ? action->isConfigurationFlagSet(WidgetAction::ConfigurationFlag::ToolButtonAutoRaise) : false);

    _layout.addWidget(&_toolButton);

    setLayout(&_layout);

    setAction(action);
}

WidgetActionCollapsedWidget::ToolButton::ToolButton(WidgetAction* action, QWidget* parent /*= nullptr*/) :
    QToolButton(parent),
    _action(action),
    _showIndicator(true),
    _indicatorAlignment(Qt::AlignBottom | Qt::AlignRight)
{
}

void WidgetActionCollapsedWidget::ToolButton::paintEvent(QPaintEvent* paintEvent)
{
    QToolButton::paintEvent(paintEvent);

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    if (getShowIndicator()) {
        const auto margin = 5.0f;

        QPointF center;

        if (_indicatorAlignment & Qt::AlignTop)
            center.setY(margin);

        if (_indicatorAlignment & Qt::AlignVCenter)
            center.setY(0.5f * height());

        if (_indicatorAlignment & Qt::AlignBottom)
            center.setY(height() - margin);

        if (_indicatorAlignment & Qt::AlignLeft)
            center.setX(margin);

        if (_indicatorAlignment & Qt::AlignHCenter)
            center.setX(0.5f * width());

        if (_indicatorAlignment & Qt::AlignRight)
            center.setX(width() - margin);

        painter.setPen(QPen(QBrush(isEnabled() ? Qt::black : Qt::gray), 2.5, Qt::SolidLine, Qt::RoundCap));
        painter.setBrush(Qt::NoBrush);
        painter.drawPoint(center);
    }
}

}
