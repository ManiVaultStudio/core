// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WidgetActionToolButton.h"

#include "StatusBarAction.h"
#include "WidgetAction.h"

#include <QDebug>
#include <QPainter>

namespace mv::gui {

QPushButton* WidgetActionToolButton::sizeHintPushButton = nullptr;

WidgetActionToolButton::WidgetActionToolButton(QWidget* parent, WidgetAction* action) :
    QToolButton(parent),
    _action(nullptr),
    _showIndicator(true),
    _indicatorAlignment(Qt::AlignBottom | Qt::AlignRight),
    _menu(*this)
{
    initialize(action);
    
    if (!WidgetActionToolButton::sizeHintPushButton) {
        WidgetActionToolButton::sizeHintPushButton = new QPushButton(" ");
        sizeHintPushButton->ensurePolished();
    }
    
    const auto heightHint = WidgetActionToolButton::sizeHintPushButton->sizeHint().height();
    
    setMinimumSize(QSize(heightHint, heightHint));

    setIconSize(QSize(12, 12));
}

WidgetActionToolButton::WidgetActionToolButton(QWidget* parent, WidgetAction* action, WidgetConfigurationFunction widgetConfigurationFunction) :
    WidgetActionToolButton(parent, action)
{
    _widgetConfigurationFunction = widgetConfigurationFunction;
}

void WidgetActionToolButton::paintEvent(QPaintEvent* paintEvent)
{
    QToolButton::paintEvent(paintEvent);

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    if (getShowIndicator()) {
	    constexpr auto margin = 5.0f;

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

        painter.setPen(QPen(QBrush(isEnabled() ? qApp->palette().text().color() : Qt::gray), 2.5, Qt::SolidLine, Qt::RoundCap));
        painter.setBrush(Qt::NoBrush);
        painter.drawPoint(center);
    }

    auto rect = QRect({}, iconSize());

    rect.moveCenter(geometry().center());

    painter.drawPixmap(rect, _action->icon().pixmap(iconSize()));
}

WidgetAction* WidgetActionToolButton::getAction() const
{
    return _action;
}

void WidgetActionToolButton::setAction(WidgetAction* action)
{
    auto previousAction = _action;

    if (_action) {
        disconnect(_action, &WidgetAction::changed, this, nullptr);
        disconnect(_action, &WidgetAction::enabledChanged, this, nullptr);
        disconnect(_action, &WidgetAction::visibleChanged, this, nullptr);
    }

    _action = action;

    if (_action) {
        const auto actionChanged = [this]() {
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
    }
    
    setAutoRaise(_action ? _action->isConfigurationFlagSet(WidgetAction::ConfigurationFlag::ToolButtonAutoRaise) : false);

    emit actionChanged(previousAction, _action);
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
    setMenu(&_menu);
}

}
