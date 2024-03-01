// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WidgetActionToolButton.h"
#include "WidgetAction.h"

#include <QDebug>
#include <QPainter>

namespace mv::gui {

WidgetActionToolButton::WidgetActionToolButton(QWidget* parent, WidgetAction* action) :
    QToolButton(parent),
    _action(nullptr),
    _widgetConfigurationFunction(),
    _showIndicator(true),
    _indicatorAlignment(Qt::AlignBottom | Qt::AlignRight)
{
    initialize(action);
}

WidgetActionToolButton::WidgetActionToolButton(QWidget* parent, WidgetAction* action, WidgetConfigurationFunction widgetConfigurationFunction) :
    QToolButton(parent),
    _action(nullptr),
    _widgetConfigurationFunction(),
    _showIndicator(true),
    _indicatorAlignment(Qt::AlignBottom | Qt::AlignRight)
{
    _widgetConfigurationFunction = widgetConfigurationFunction;

    initialize(action);
}

void WidgetActionToolButton::paintEvent(QPaintEvent* paintEvent)
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

WidgetAction* WidgetActionToolButton::getAction()
{
    return _action;
}

void WidgetActionToolButton::setAction(WidgetAction* action)
{
    if (_action) {
        removeAction(_action);

        disconnect(_action, &WidgetAction::changed, this, nullptr);
        disconnect(_action, &WidgetAction::enabledChanged, this, nullptr);
        disconnect(_action, &WidgetAction::visibleChanged, this, nullptr);

        _action->setProperty("Popup", true);
    }

    _action = action;

    if (_action) {
        addAction(_action);

        const auto actionChanged = [this]() {
            setIcon(_action->icon());
            setToolTip(_action->toolTip());
        };

        const auto actionEnabledChanged = [this]() {
            setEnabled(_action->isEnabled());
        };

        const auto actionVisibilityChanged = [this]() {
            setVisible(_action->isVisible());
        };

        actionChanged();
        actionEnabledChanged();
        actionVisibilityChanged();

        connect(_action, &WidgetAction::changed, this, actionChanged);
        connect(_action, &WidgetAction::enabledChanged, this, actionEnabledChanged);
        connect(_action, &WidgetAction::visibleChanged, this, actionVisibilityChanged);

        _action->setProperty("Popup", true);
    }

    setAutoRaise(_action ? _action->isConfigurationFlagSet(WidgetAction::ConfigurationFlag::ToolButtonAutoRaise) : false);
}

WidgetConfigurationFunction WidgetActionToolButton::getWidgetConfigurationFunction()
{
    return _widgetConfigurationFunction;
}

void WidgetActionToolButton::initialize(WidgetAction* action)
{
    setObjectName("WidgetActionToolButton");
    setPopupMode(QToolButton::InstantPopup);
    setIconSize(QSize(16, 16));
    setStyleSheet("QToolButton::menu-indicator { image: none; }");
    setAction(action);
}

}
