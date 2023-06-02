#include "ToolbarAction.h"

namespace hdps::gui {

#if (__cplusplus < 201703L)   // definition needed for pre C++17 gcc and clang
    constexpr std::int32_t ToolbarAction::CONTENTS_MARGIN;
#endif

ToolbarAction::ToolbarAction(QObject* parent, const QString& title) :
    WidgetAction(parent, title),
    _groupAction(this, "Actions"),
    _actionItems()
{
    setText(title);
}

GroupAction& ToolbarAction::getGroupAction()
{
    return _groupAction;
}

void ToolbarAction::addAction(WidgetAction* action, const std::int32_t& autoExpandPriority /*= -1*/)
{
    _groupAction.addAction(action);

    _actionItems[action] = new ToolbarActionItem(this, action, ToolbarActionItem::State::Collapsed, autoExpandPriority);

    emit actionWidgetsChanged(_actionItems);
}

void ToolbarAction::removeAction(WidgetAction* action)
{
    _groupAction.removeAction(action);

    if (_actionItems.contains(action))
        _actionItems.remove(action);

    emit actionWidgetsChanged(_actionItems);
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

    if (!_actionItems.contains(action))
        return;

    _actionItems[action]->setAutoExpandPriority(autoExpandPriority);
}

ToolbarAction::ActionItems& ToolbarAction::getActionItems()
{
    return _actionItems;
}

}
