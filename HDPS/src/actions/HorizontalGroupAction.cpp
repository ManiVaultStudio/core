#include "HorizontalGroupAction.h"

#include <QHBoxLayout>
#include <QSet>

namespace hdps::gui {

HorizontalGroupAction::HorizontalGroupAction(QObject* parent, const QString& title /*= ""*/) :
    WidgetAction(parent),
    _actions(),
    _showLabels(false)
{
    setText(title);
}

QString HorizontalGroupAction::getTypeString() const
{
    return "HorizontalGroup";
}

bool HorizontalGroupAction::getShowLabels() const
{
    return _showLabels;
}

void HorizontalGroupAction::setShowLabels(bool showLabels)
{
    if (showLabels == _showLabels)
        return;

    _showLabels = showLabels;

    emit showLabelsChanged(_showLabels);
}

void HorizontalGroupAction::setActions(const ConstWidgetActions& actions)
{
    _actions = actions;

    emit actionsChanged(_actions);
}

ConstWidgetActions HorizontalGroupAction::getActions()
{
    return _actions;
}

void HorizontalGroupAction::addAction(const WidgetAction& action)
{
    _actions << &action;
}

HorizontalGroupAction::Widget::Widget(QWidget* parent, HorizontalGroupAction* horizontalGroupAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, horizontalGroupAction, widgetFlags),
    _HorizontalGroupAction(horizontalGroupAction)
{
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    for (auto action : horizontalGroupAction->getActions()) {
        if (horizontalGroupAction->getShowLabels())
            layout->addWidget(action->createLabelWidget(this));

        layout->addWidget(const_cast<WidgetAction*>(action)->createWidget(this), action->getStretch());
    }

    setLayout(layout);
}

}
