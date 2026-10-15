// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WidgetActionToolButtonMenu.h"

#include "WidgetAction.h"
#include "WidgetActionToolButton.h"

#include <QActionEvent>
#include <QCloseEvent>
#include <QCoreApplication>
#include <QDebug>
#include <QEvent>
#include <QVBoxLayout>

namespace mv::gui {

WidgetActionToolButtonMenu::WidgetActionToolButtonMenu(WidgetActionToolButton& widgetActionToolButton) :
    QMenu(&widgetActionToolButton),
    _widgetActionToolButton(widgetActionToolButton),
    _deferredLoadWidgetAction(widgetActionToolButton),
    _ignoreCloseEvent(false)
{
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    addAction(&_deferredLoadWidgetAction);

    connect(this, &QMenu::aboutToShow, this, [this]() -> void {
        if (auto currentAction = _widgetActionToolButton.getAction())
            _deferredLoadWidgetAction.getActionWidget().initialize();
    });
}

WidgetConfigurationFunction WidgetActionToolButtonMenu::getWidgetConfigurationFunction()
{
    return _widgetConfigurationFunction;
}

void WidgetActionToolButtonMenu::closeEvent(QCloseEvent* event)
{
    if (_ignoreCloseEvent)
        event->ignore();
}

bool WidgetActionToolButtonMenu::getIgnoreCloseEvent() const
{
    return _ignoreCloseEvent;
}

void WidgetActionToolButtonMenu::setIgnoreCloseEvent(bool ignoreCloseEvent)
{
    _ignoreCloseEvent = ignoreCloseEvent;
}

WidgetActionToolButtonMenu::DeferredLoadWidgetAction::DeferredLoadWidgetAction(WidgetActionToolButton& widgetActionToolButton) :
    QWidgetAction(&widgetActionToolButton),
    _widgetActionToolButton(widgetActionToolButton),
    _actionWidget(_widgetActionToolButton)
{
}

QWidget* WidgetActionToolButtonMenu::DeferredLoadWidgetAction::createWidget(QWidget* parent)
{
    return &_actionWidget;
}

WidgetActionToolButtonMenu::DeferredLoadWidgetAction::ActionWidget::ActionWidget(WidgetActionToolButton& widgetActionToolButton) :
    QWidget(&widgetActionToolButton.getMenu()),
    _widgetActionToolButton(widgetActionToolButton),
    _widget(nullptr)
{
    setProperty("Popup", true);

    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    setLayout(layout);

    // _widget for a button is built for one specific action.
    // If a different action is assigned to the button, it needs to be invalidated.
    connect(&_widgetActionToolButton, &WidgetActionToolButton::actionChanged, this, [this]() -> void {
        delete _widget;
        _widget = nullptr;
    });
}

QSize WidgetActionToolButtonMenu::DeferredLoadWidgetAction::ActionWidget::sizeHint() const
{
    // Most actions leave the popup size hint unset.
    // If the contained action changes this is needed to resize properly
    if (auto currentAction = _widgetActionToolButton.getAction()) {
        const auto popupSizeHint = currentAction->getPopupSizeHint();

        if (popupSizeHint.isValid())
            return popupSizeHint;
    }

    if (_widget)
        return _widget->sizeHint();

    return {};
}

void WidgetActionToolButtonMenu::DeferredLoadWidgetAction::ActionWidget::initialize()
{
    if (auto currentAction = _widgetActionToolButton.getAction()) {
        if (!_widget) {
            _widget = currentAction->createWidget(this);

            _widget->setProperty("Popup", true);

            layout()->addWidget(_widget);

            // QMenu measures its items once. Changing the contained action needs to be communicated to to update geometry aftrer init.
            auto& menu = _widgetActionToolButton.getMenu();

            QActionEvent actionChangedEvent(QEvent::ActionChanged, &menu.getDeferredLoadWidgetAction());

            QCoreApplication::sendEvent(&menu, &actionChangedEvent);
        }
    }
}

}
