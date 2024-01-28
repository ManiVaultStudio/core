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
    _toolButton()
{
    _layout.setContentsMargins(0, 0, 0, 0);

    _toolButton.setPopupMode(QToolButton::InstantPopup);
    _toolButton.setIconSize(QSize(16, 16));
    _toolButton.setStyleSheet("QToolButton::menu-indicator { image: none; }");

    _layout.addWidget(&_toolButton);

    setLayout(&_layout);
    
    setAction(action);
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

void WidgetActionCollapsedWidget::ToolButton::paintEvent(QPaintEvent* paintEvent)
{
    QToolButton::paintEvent(paintEvent);

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);

    const auto margin = 5.0f;

    QPointF center;

    center.setY(height() - margin);
    center.setX(width() - margin);

    painter.setPen(QPen(QBrush(isEnabled() ? Qt::black : Qt::gray), 2.5, Qt::SolidLine, Qt::RoundCap));
    painter.setBrush(Qt::NoBrush);
    painter.drawPoint(center);
}

}
