#include "HorizontalGroupAction.h"

#include <QHBoxLayout>
#include <QSet>

namespace hdps::gui {

HorizontalGroupAction::HorizontalGroupAction(QObject* parent, const QString& title /*= ""*/) :
    WidgetAction(parent),
    _actions(),
    _showLabels(true)
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

    connect(&action, &WidgetAction::configurationFlagToggled, this, [this](const WidgetAction::ConfigurationFlag& configurationFlag, bool set) -> void {
        if (configurationFlag != WidgetAction::ConfigurationFlag::NoLabelInGroup)
            return;

        emit actionsChanged(getActions());
    });

    emit actionsChanged(getActions());
}

void HorizontalGroupAction::removeAction(const WidgetAction& action)
{
    if (!_actions.contains(&action))
        return;

    _actions.removeOne(&action);

    disconnect(&action, &WidgetAction::configurationFlagToggled, this, nullptr);
}

HorizontalGroupAction::Widget::Widget(QWidget* parent, HorizontalGroupAction* horizontalGroupAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, horizontalGroupAction, widgetFlags),
    _HorizontalGroupAction(horizontalGroupAction)
{
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    const auto updateLayout = [&]() -> void {
        QLayoutItem* layoutItem;

        while ((layoutItem = layout->takeAt(0)) != nullptr) {
            delete layoutItem->widget();
            delete layoutItem;
        }

        for (auto action : horizontalGroupAction->getActions()) {
            if (horizontalGroupAction->getShowLabels() && !action->isConfigurationFlagSet(WidgetAction::ConfigurationFlag::NoLabelInGroup))
                layout->addWidget(action->createLabelWidget(this));

            layout->addWidget(const_cast<WidgetAction*>(action)->createWidget(this), action->getStretch());
        }
    };

    updateLayout();

    connect(horizontalGroupAction, &HorizontalGroupAction::actionsChanged, this, updateLayout);

    setLayout(layout);
}

}
