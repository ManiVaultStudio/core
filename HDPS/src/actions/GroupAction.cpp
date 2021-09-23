#include "GroupAction.h"
#include "WidgetActionLabel.h"
#include "ToggleAction.h"
#include "TriggerAction.h"

#include <QDebug>
#include <QGridLayout>

namespace hdps {

namespace gui {

GroupAction::GroupAction(QObject* parent, const bool& expanded /*= false*/) :
    WidgetAction(parent),
    _expanded(expanded),
    _readOnly(false)
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

GroupAction::FormWidget::FormWidget(QWidget* parent, GroupAction* groupAction) :
    WidgetActionWidget(parent, groupAction, WidgetActionWidget::State::Standard),
    _layout(new QGridLayout())
{
    _layout->setColumnStretch(0, 2);
    _layout->setColumnStretch(1, 5);

    auto contentsMargin = _layout->contentsMargins();
    
    _layout->setMargin(5);

    setLayout(_layout);

    for (auto child : groupAction->children()) {
        auto childWidgetAction = dynamic_cast<WidgetAction*>(child);

        if (childWidgetAction == nullptr)
            continue;

        const auto numRows = _layout->rowCount();

        const auto isToggleAction   = dynamic_cast<ToggleAction*>(childWidgetAction);
        const auto isTriggerAction  = dynamic_cast<TriggerAction*>(childWidgetAction);

        if (!isToggleAction && !isTriggerAction) {
            auto labelWidget = childWidgetAction->createLabelWidget(this);
            labelWidget->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            _layout->addWidget(labelWidget, numRows, 0);
        }

        _layout->addWidget(childWidgetAction->createWidget(this), numRows, 1);
    }
}

QGridLayout* GroupAction::FormWidget::layout()
{
    return _layout;
}

}
}
