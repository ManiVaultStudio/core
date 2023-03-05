#include "ToolbarActionItem.h"
#include "ToolbarActionItemWidget.h"

namespace hdps::gui {

ToolbarActionItem::ToolbarActionItem(QObject* parent, const WidgetAction* action, const State& state /*= State::Collapsed*/, std::int32_t autoExpandPriority /*= -1*/) :
    QObject(parent),
    _action(action),
    _state(state),
    _autoExpandPriority(autoExpandPriority)
{
}

std::int32_t ToolbarActionItem::getAutoExpandPriority() const
{
    return _autoExpandPriority;
}

void ToolbarActionItem::setAutoExpandPriority(std::int32_t autoExpandPriority)
{
    if (autoExpandPriority == _autoExpandPriority)
        return;

    _autoExpandPriority = autoExpandPriority;
}

QWidget* ToolbarActionItem::createWidget(QWidget* parent)
{
    return new ToolbarActionItemWidget(parent, *this);
}

bool ToolbarActionItem::eventFilter(QObject* target, QEvent* event)
{
    return QObject::eventFilter(target, event);
}

ToolbarActionItem::State ToolbarActionItem::getState() const
{
    return _state;
}

void ToolbarActionItem::setState(const State& state)
{
    if (state == _state)
        return;

    _state = state;

    emit stateChanged(_state);
}

const WidgetAction* ToolbarActionItem::getAction()
{
    return _action;
}

}
