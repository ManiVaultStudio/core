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
    _deferredLoadWidgetAction(widgetActionToolButton),
    _widgetConfigurationFunction()
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
}

QSize WidgetActionToolButtonMenu::DeferredLoadWidgetAction::ActionWidget::sizeHint() const
{
    if (auto currentAction = _widgetActionToolButton.getAction())
        return currentAction->getPopupSizeHint();

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
        }
    }
}

}
