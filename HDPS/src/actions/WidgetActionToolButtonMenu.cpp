// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WidgetActionToolButtonMenu.h"
#include "WidgetActionToolButton.h"
#include "WidgetAction.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QEvent>

namespace mv::gui {

WidgetActionToolButtonMenu::WidgetActionToolButtonMenu(WidgetActionToolButton& widgetActionToolButton) :
    QMenu(&widgetActionToolButton),
    _widgetActionToolButton(widgetActionToolButton),
    _widgetAction(widgetActionToolButton),
    _widgetConfigurationFunction(),
    _widget(nullptr)
{
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    addAction(&_widgetAction);

    connect(this, &QMenu::aboutToShow, this, [this]() -> void {
        if (auto currentAction = _widgetActionToolButton.getAction())
            _widgetAction.getActionWidget().initialize();
    });
}

WidgetConfigurationFunction WidgetActionToolButtonMenu::getWidgetConfigurationFunction()
{
    return _widgetConfigurationFunction;
}

WidgetActionToolButtonMenu::DeferredWidgetAction::DeferredWidgetAction(WidgetActionToolButton& widgetActionToolButton) :
    QWidgetAction(&widgetActionToolButton),
    _widgetActionToolButton(widgetActionToolButton),
    _actionWidget(&_widgetActionToolButton.getMenu(), _widgetActionToolButton)
{
}

QWidget* WidgetActionToolButtonMenu::DeferredWidgetAction::createWidget(QWidget* parent)
{
    return &_actionWidget;
}

WidgetActionToolButtonMenu::DeferredWidgetAction::ActionWidget::ActionWidget(QWidget* parent, WidgetActionToolButton& widgetActionToolButton) :
    QWidget(parent),
    _widgetActionToolButton(widgetActionToolButton),
    _widget(nullptr)
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    setLayout(layout);
}

QSize WidgetActionToolButtonMenu::DeferredWidgetAction::ActionWidget::sizeHint() const
{
    if (auto currentAction = _widgetActionToolButton.getAction())
        return currentAction->getPopupSizeHint();

    if (_widget)
        return _widget->sizeHint();

    return {};
}

void WidgetActionToolButtonMenu::DeferredWidgetAction::ActionWidget::initialize()
{
    if (auto currentAction = _widgetActionToolButton.getAction()) {
        if (!_widget) {
            _widget = currentAction->createWidget(this);

            layout()->addWidget(_widget);
        }
    }
}

}
