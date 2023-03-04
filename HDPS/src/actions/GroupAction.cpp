#include "GroupAction.h"

#include <QDebug>

namespace hdps::gui {

GroupAction::GroupAction(QObject* parent, const QString& title, const bool& expanded /*= false*/) :
    WidgetAction(parent),
    _expanded(expanded),
    _readOnly(false),
    _actions(),
    _showLabels(true)
{
    setText(title);
}

void GroupAction::setExpanded(const bool& expanded)
{
    if (expanded == _expanded)
        return;

    if (expanded)
        expand();
    else
        collapse();
}

void GroupAction::expand()
{
    if (_expanded == true)
        return;

    _expanded = true;

    emit expanded();
}

void GroupAction::collapse()
{
    if (_expanded == false)
        return;

    _expanded = false;

    emit collapsed();
}

void GroupAction::toggle()
{
    setExpanded(!isExpanded());
}

bool GroupAction::isExpanded() const
{
    return _expanded;
}

bool GroupAction::isCollapsed() const
{
    return !_expanded;
}

bool GroupAction::isReadOnly() const
{
    return _readOnly;
}

void GroupAction::setReadOnly(const bool& readOnly)
{
    if (readOnly == _readOnly)
        return;

    _readOnly = readOnly;

    emit readOnlyChanged(_readOnly);
}

bool GroupAction::getShowLabels() const
{
    return _showLabels;
}

void GroupAction::setShowLabels(bool showLabels)
{
    if (showLabels == _showLabels)
        return;

    _showLabels = showLabels;

    emit showLabelsChanged(_showLabels);
}

void GroupAction::addAction(const WidgetAction* action)
{
    _actions << action;

    sortActions();

    QList<std::int32_t> configurationFlagsRequireUpdate{
        static_cast<std::int32_t>(WidgetAction::ConfigurationFlag::NoLabelInGroup),
        static_cast<std::int32_t>(WidgetAction::ConfigurationFlag::AlwaysCollapsed)
    };

    connect(action, &WidgetAction::configurationFlagToggled, this, [&](const WidgetAction::ConfigurationFlag& configurationFlag, bool set) -> void {
        if (!configurationFlagsRequireUpdate.contains(static_cast<std::int32_t>(configurationFlag)))
            return;

        emit actionsChanged(getActions());
    });

    connect(action, &WidgetAction::sortIndexChanged, this, [&](std::int32_t sortIndex) -> void {
        sortActions();

        emit actionsChanged(getActions());
    });

    emit actionsChanged(getActions());
}

void GroupAction::removeAction(const WidgetAction* action)
{
    if (!_actions.contains(action))
        return;

    _actions.removeOne(action);

    disconnect(action, &WidgetAction::configurationFlagToggled, this, nullptr);
    disconnect(action, &WidgetAction::sortIndexChanged, this, nullptr);
}

ConstWidgetActions GroupAction::getActions()
{
    return _actions;
}

void GroupAction::setActions(const ConstWidgetActions& actions)
{
    _actions = actions;

    emit actionsChanged(_actions);
}

void GroupAction::sortActions()
{
    std::sort(_actions.begin(), _actions.end(), [](const WidgetAction* lhs, const WidgetAction* rhs) {
        return rhs->getSortIndex() > lhs->getSortIndex();
    });
}

}
