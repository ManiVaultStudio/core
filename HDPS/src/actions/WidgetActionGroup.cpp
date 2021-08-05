#include "WidgetActionGroup.h"
#include "ToggleAction.h"

#include <QDebug>
#include <QMenu>
#include <QEvent>
#include <QPainter>

namespace hdps {

namespace gui {

WidgetActionGroup::WidgetActionGroup(QObject* parent, const bool& expanded /*= false*/) :
    WidgetAction(parent),
    _expanded(expanded)
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

WidgetActionGroup::GroupWidget::GroupWidget(QWidget* parent, WidgetActionGroup* widgetActionGroup) :
    WidgetAction::Widget(parent, widgetActionGroup, Widget::State::Standard),
    _layout(new QGridLayout())
{
    _layout->setColumnStretch(0, 1);
    _layout->setColumnStretch(1, 2);

    setLayout(_layout);
}

void WidgetActionGroup::GroupWidget::addWidgetAction(WidgetAction& widgetAction)
{
    const auto numRows = _layout->rowCount();

    auto toggleAction = dynamic_cast<ToggleAction*>(&widgetAction);

    if (toggleAction) {
        _layout->addWidget(toggleAction->createCheckBoxWidget(this), numRows, 1);
    }
    else {
        _layout->addWidget(widgetAction.createLabelWidget(this), numRows, 0);
        _layout->addWidget(widgetAction.createWidget(this), numRows, 1);
    }
}

QGridLayout* WidgetActionGroup::GroupWidget::layout()
{
    return _layout;
}

}
}
