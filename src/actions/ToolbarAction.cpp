// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ToolbarAction.h"

namespace mv::gui {

ToolbarAction::ToolbarAction(QObject* parent, const QString& title, const Qt::AlignmentFlag& alignment /*= Qt::AlignmentFlag::AlignLeft*/) :
    WidgetAction(parent, title),
    _groupAction(this, "Actions", false, alignment),
    _actionItems(),
    _actionItemsMap()
{
}

GroupAction& ToolbarAction::getGroupAction()
{
    return _groupAction;
}

void ToolbarAction::addAction(WidgetAction* action, const std::int32_t& autoExpandPriority /*= -1*/, std::int32_t widgetFlags /*= -1*/, WidgetConfigurationFunction widgetConfigurationFunction /*= WidgetConfigurationFunction()*/)
{
    Q_ASSERT(action != nullptr);

    if (action == nullptr)
        return;

    if (widgetFlags == -1)
        widgetFlags = action->getDefaultWidgetFlags();

    _groupAction.addAction(action, widgetFlags, widgetConfigurationFunction);

    auto actionItem = new ToolbarActionItem(this, action, widgetFlags, ToolbarActionItem::State::Collapsed, autoExpandPriority);

    _actionItems << actionItem;

    _actionItemsMap[action] = actionItem;

    connect(_actionItemsMap[action], &ToolbarActionItem::autoExpandPriorityChanged, this, &ToolbarAction::layoutInvalidated);
    connect(_actionItemsMap[action], &ToolbarActionItem::widgetSizeChanged, this, &ToolbarAction::layoutInvalidated);

    emit actionWidgetsChanged(_actionItemsMap);
}

void ToolbarAction::removeAction(WidgetAction* action)
{
    _groupAction.removeAction(action);

    if (_actionItemsMap.contains(action)) {
        auto actionItem = _actionItemsMap[action];

        disconnect(actionItem, &ToolbarActionItem::autoExpandPriorityChanged, this, nullptr);
        disconnect(actionItem, &ToolbarActionItem::widgetSizeChanged, this, nullptr);

        if (_actionItems.contains(actionItem))
            _actionItems.removeOne(actionItem);

        _actionItemsMap.remove(action);

        emit actionWidgetsChanged(_actionItemsMap);
    }
}

StretchAction* ToolbarAction::addStretch(std::int32_t stretch /*= 1*/)
{
    auto stretchAction = new StretchAction(this, "Stretch", stretch);

    addAction(stretchAction);

    return stretchAction;
}

WidgetActions ToolbarAction::getActions()
{
    return _groupAction.getActions();
}

void ToolbarAction::setActionAutoExpandPriority(const WidgetAction* action, const std::int32_t& autoExpandPriority)
{
    Q_ASSERT(action != nullptr);

    if (action == nullptr)
        return;

    if (!_actionItemsMap.contains(action))
        return;

    _actionItemsMap[action]->setAutoExpandPriority(autoExpandPriority);
}

ToolbarAction::ActionItems& ToolbarAction::getActionItems()
{
    return _actionItems;
}

void ToolbarAction::invalidateLayout()
{
    emit layoutInvalidated();
}

}
