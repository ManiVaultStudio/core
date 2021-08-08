#include "WidgetActionGroup.h"
#include "ToggleAction.h"
#include "TriggerAction.h"

#include <QDebug>
#include <QMenu>
#include <QEvent>
#include <QPainter>

namespace hdps {

namespace gui {

WidgetActionGroup::WidgetActionGroup(QObject* parent, const bool& expanded /*= false*/) :
    WidgetAction(parent),
    _expanded(expanded),
    _readOnly(false)
{
}

void WidgetActionGroup::setExpanded(const bool& expanded)
{
    if (expanded == _expanded)
        return;

    if (expanded)
        expand();
    else
        collapse();
}

void WidgetActionGroup::expand()
{
    if (_expanded == true)
        return;

    _expanded = true;

    emit expanded();
}

void WidgetActionGroup::collapse()
{
    if (_expanded == false)
        return;

    _expanded = false;

    emit collapsed();
}

void WidgetActionGroup::toggle()
{
    setExpanded(!isExpanded());
}

bool WidgetActionGroup::isExpanded() const
{
    return _expanded;
}

bool WidgetActionGroup::isCollapsed() const
{
    return !_expanded;
}

bool WidgetActionGroup::isReadOnly() const
{
    return _readOnly;
}

void WidgetActionGroup::setReadOnly(const bool& readOnly)
{
    if (readOnly == _readOnly)
        return;

    _readOnly = readOnly;

    emit readOnlyChanged(_readOnly);
}

WidgetActionGroup::GroupWidget::GroupWidget(QWidget* parent, WidgetActionGroup* widgetActionGroup) :
    WidgetAction::Widget(parent, widgetActionGroup, Widget::State::Standard),
    _layout(new QGridLayout())
{
    _layout->setColumnStretch(0, 2);
    _layout->setColumnStretch(1, 5);

    setLayout(_layout);
}

void WidgetActionGroup::GroupWidget::addWidgetAction(WidgetAction& widgetAction, const bool& forceTogglePushButton /*= false*/)
{
    const auto numRows = _layout->rowCount();

    auto toggleAction   = dynamic_cast<ToggleAction*>(&widgetAction);
    auto triggerAction  = dynamic_cast<TriggerAction*>(&widgetAction);

    if (toggleAction || triggerAction) {
        if (toggleAction) {
            if (forceTogglePushButton)
                _layout->addWidget(toggleAction->createPushButtonWidget(this), numRows, 1);
            else
                _layout->addWidget(toggleAction->createCheckBoxWidget(this), numRows, 1);
        }

        if (triggerAction != nullptr)
            _layout->addWidget(triggerAction->createWidget(this), numRows, 1);
    }
    else {
        auto labelWidget = widgetAction.createLabelWidget(this);

        labelWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        _layout->addWidget(labelWidget, numRows, 0);
        _layout->addWidget(widgetAction.createWidget(this), numRows, 1);
    }
}

QGridLayout* WidgetActionGroup::GroupWidget::layout()
{
    return _layout;
}

}
}
