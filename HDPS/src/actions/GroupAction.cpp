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

void GroupAction::setActions(const QVector<WidgetAction*>& widgetActions /*= QVector<WidgetAction*>()*/)
{
    _widgetActions = widgetActions;

    emit actionsChanged(_widgetActions);
}

QVector<WidgetAction*> GroupAction::getSortedWidgetActions() const
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
    _layout->setColumnStretch(0, 3);
    _layout->setColumnStretch(1, 5);
    _layout->setContentsMargins(10, 10, 10, 10);    

    setLayout(_layout);

    const auto actionsChanged = [this, groupAction]() -> void {
        QLayoutItem* layoutItem;

        while ((layoutItem = layout()->takeAt(0)) != NULL)
        {
            delete layoutItem->widget();
            delete layoutItem;
        }

        for (auto widgetAction : groupAction->getSortedWidgetActions()) {
            const auto numRows          = _layout->rowCount();
            const auto isToggleAction   = dynamic_cast<ToggleAction*>(widgetAction);
            const auto isTriggerAction  = dynamic_cast<TriggerAction*>(widgetAction);
            const auto isTriggersAction = dynamic_cast<TriggersAction*>(widgetAction);

            if (!isToggleAction && !isTriggerAction && !isTriggersAction) {
                auto labelWidget = dynamic_cast<WidgetActionLabel*>(widgetAction->createLabelWidget(this, WidgetActionLabel::ColonAfterName));
                _layout->addWidget(labelWidget, numRows, 0);
            }

            auto actionWidget = widgetAction->createWidget(this);

            _layout->addWidget(actionWidget, numRows, 1);

            if (isToggleAction)
                _layout->setAlignment(actionWidget, Qt::AlignLeft);
        }
    };

    // Update UI when the group actions change
    connect(groupAction, &GroupAction::actionsChanged, this, actionsChanged);

    // Initial update of the actions
    actionsChanged();
}

QGridLayout* GroupAction::FormWidget::layout()
{
    return _layout;
}

}
}
