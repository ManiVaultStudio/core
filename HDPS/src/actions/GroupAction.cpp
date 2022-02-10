#include "GroupAction.h"
#include "WidgetActionLabel.h"
#include "ToggleAction.h"
#include "TriggerAction.h"
#include "TriggersAction.h"

#include <QDebug>
#include <QGridLayout>

namespace hdps {

namespace gui {

GroupAction::GroupAction(QObject* parent, const bool& expanded /*= false*/) :
    WidgetAction(parent),
    _expanded(expanded),
    _readOnly(false),
    _widgetActions()
{
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

QVector<WidgetAction*> GroupAction::getSortedWidgetActions()
{
    auto sortedActions = _widgetActions;

    for (auto child : children()) {
        auto childWidgetAction = dynamic_cast<WidgetAction*>(child);

        if (childWidgetAction == nullptr)
            continue;

        if (!childWidgetAction->isVisible())
            continue;

        sortedActions << childWidgetAction;
    }

    std::sort(sortedActions.begin(), sortedActions.end(), [](WidgetAction* lhs, WidgetAction* rhs) {
        return rhs->getSortIndex() > lhs->getSortIndex();
    });

    return sortedActions;
}

GroupAction::FormWidget::FormWidget(QWidget* parent, GroupAction* groupAction) :
    WidgetActionWidget(parent, groupAction),
    _layout(new QGridLayout())
{
    _layout->setColumnStretch(0, 2);
    _layout->setColumnStretch(1, 5);

    auto contentsMargin = _layout->contentsMargins();
    
    _layout->setContentsMargins(15,15,15,15);

    setLayout(_layout);

    for (auto widgetAction : groupAction->getSortedWidgetActions()) {
        const auto numRows          = _layout->rowCount();
        const auto isToggleAction   = dynamic_cast<ToggleAction*>(widgetAction);
        const auto isTriggerAction  = dynamic_cast<TriggerAction*>(widgetAction);
        const auto isTriggersAction = dynamic_cast<TriggersAction*>(widgetAction);

        if (!isToggleAction && !isTriggerAction && !isTriggersAction) {
            auto labelWidget = widgetAction->createLabelWidget(this);
            labelWidget->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            _layout->addWidget(labelWidget, numRows, 0);
        }

        _layout->addWidget(widgetAction->createWidget(this), numRows, 1);

        if (widgetAction->getMayReset())
            _layout->addWidget(widgetAction->createResetButton(this), numRows, 2);
    }
}

QGridLayout* GroupAction::FormWidget::layout()
{
    return _layout;
}

}
}
