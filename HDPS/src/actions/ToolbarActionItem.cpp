#include "ToolbarActionItem.h"
#include "ToolbarActionItemWidget.h"
#include "WidgetAction.h"

namespace hdps::gui {

ToolbarActionItem::ToolbarActionItem(QObject* parent, const WidgetAction* action, const State& state /*= State::Collapsed*/, std::int32_t autoExpandPriority /*= -1*/) :
    QObject(parent),
    _action(action),
    _state(state),
    _autoExpandPriority(autoExpandPriority),
    _widgetSizes()
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

    emit autoExpandPriorityChanged(_autoExpandPriority);
}

QWidget* ToolbarActionItem::createWidget(QWidget* parent)
{
    return new ToolbarActionItemWidget(parent, *this);
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

    qDebug() << __FUNCTION__ << getAction()->text() << static_cast<int>(state);

    emit stateChanged(_state);
}

const WidgetAction* ToolbarActionItem::getAction()
{
    return _action;
}

QSize ToolbarActionItem::getWidgetSize(const State& state) const
{
    return _widgetSizes[static_cast<std::int32_t>(state)];
}

void ToolbarActionItem::setWidgetSize(const QSize& size, const State& state)
{
    _widgetSizes[static_cast<std::int32_t>(state)] = size;

    emit widgetSizeChanged(size, state);
}

}
