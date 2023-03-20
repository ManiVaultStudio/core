#include "ToolbarAction.h"

namespace hdps::gui {

//ToolbarAction::ToolbarAction(QObject* parent, const QString& title /*= "Horizontal Toolbar"*/) :
//    WidgetAction(parent),
//    _groupAction(this, "Actions"),
//    _actionItems()
//{
//    setText(title);
//}
//
//QString ToolbarAction::getTypeString() const
//{
//    return "Toolbar";
//}
//
//GroupAction& ToolbarAction::getGroupAction()
//{
//    return _groupAction;
//}
//
//void ToolbarAction::addAction(const WidgetAction* action, const std::int32_t& autoExpandPriority /*= -1*/)
//{
//    _groupAction.addAction(action);
//
//    _actionItems[action] = new ToolbarActionItem(this, action, ToolbarActionItem::State::Collapsed, autoExpandPriority);
//
//    emit actionWidgetsChanged(_actionItems);
//}
//
//void ToolbarAction::removeAction(const WidgetAction* action)
//{
//    _groupAction.removeAction(action);
//
//    if (_actionItems.contains(action))
//        _actionItems.remove(action);
//
//    emit actionWidgetsChanged(_actionItems);
//}
//
//ConstWidgetActions ToolbarAction::getActions()
//{
//    return _groupAction.getActions();
//}
//
//void ToolbarAction::setActionAutoExpandPriority(const WidgetAction* action, const std::int32_t& autoExpandPriority)
//{
//    Q_ASSERT(action != nullptr);
//
//    if (action == nullptr)
//        return;
//
//    if (!_actionItems.contains(action))
//        return;
//
//    _actionItems[action]->setAutoExpandPriority(autoExpandPriority);
//}
//
//ToolbarAction::ActionItems& ToolbarAction::getActionItems()
//{
//    return _actionItems;
//}

}
