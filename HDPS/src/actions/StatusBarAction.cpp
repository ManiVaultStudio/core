// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StatusBarAction.h"

#ifdef _DEBUG
    #define STATUS_BAR_ACTION_VERBOSE
#endif

#include <QDebug>

namespace mv::gui {

StatusBarAction::StatusBarAction(QObject* parent, const QString& title) :
    WidgetAction(parent, title),
    _barAction(nullptr),
    _popupAction(nullptr)
{
}

QWidget* StatusBarAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    return new Widget(parent, this, widgetFlags);
}

void StatusBarAction::setBarAction(WidgetAction* barAction)
{
    Q_ASSERT(barAction != nullptr);

    if (!barAction)
        return;

    _barAction = barAction;
}

void StatusBarAction::setPopupAction(WidgetAction* popupAction)
{
    Q_ASSERT(popupAction != nullptr);

    if (!popupAction)
        return;

    _popupAction = popupAction;
}

mv::gui::WidgetAction* StatusBarAction::getPopupAction()
{
    return _popupAction;
}

mv::gui::WidgetAction* StatusBarAction::getBarAction()
{
    return _barAction;
}

StatusBarAction::Widget::Widget(QWidget* parent, StatusBarAction* statusBarAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, statusBarAction, widgetFlags),
    _statusBarAction(statusBarAction),
    _toolButton(this, statusBarAction)
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(&_toolButton);

    setLayout(layout);

    _toolButton.setObjectName("ToolButton");
    _toolButton.setPopupMode(QToolButton::InstantPopup);
    _toolButton.setIconSize(QSize(16, 16));
    _toolButton.setStyleSheet("QToolButton::menu-indicator { image: none; }");
    _toolButton.setAutoRaise(true);
    _toolButton.addAction(_statusBarAction->getPopupAction());
    _toolButton.setFixedWidth(400);
}

StatusBarAction::Widget::ToolButton::ToolButton(QWidget* parent, StatusBarAction* statusBarAction) :
    QToolButton(parent),
    _statusBarAction(statusBarAction)
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(1, 1, 1, 1);
    layout->addWidget(statusBarAction->getBarAction()->createWidget(this));

    setLayout(layout);
}

}
