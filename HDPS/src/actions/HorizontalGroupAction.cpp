#include "HorizontalGroupAction.h"

#include <QHBoxLayout>
#include <QSet>

namespace hdps::gui {

HorizontalGroupAction::HorizontalGroupAction(QObject* parent, const QString& title) :
    WidgetAction(parent, title),
    _actions(),
    _widgetFlagsMap(),
    _showLabels(true)
{
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

//void HorizontalGroupAction::setActions(const ConstWidgetActions& actions)
//{
//    _actions = actions;
//
//    emit actionsChanged(_actions);
//}

ConstWidgetActions HorizontalGroupAction::getActions()
{
    return _actions;
}

void HorizontalGroupAction::addAction(const WidgetAction* action, std::int32_t widgetFlags /*= -1*/)
{
    _actions << action;

    _widgetFlagsMap[action] = widgetFlags;

    QList<std::int32_t> configurationFlagsRequireUpdate{
        static_cast<std::int32_t>(WidgetAction::ConfigurationFlag::NoLabelInGroup),
        static_cast<std::int32_t>(WidgetAction::ConfigurationFlag::AlwaysCollapsed)
    };

    connect(action, &WidgetAction::configurationFlagToggled, this, [&](const WidgetAction::ConfigurationFlag& configurationFlag, bool set) -> void {
        if (!configurationFlagsRequireUpdate.contains(static_cast<std::int32_t>(configurationFlag)))
            return;

        emit actionsChanged(getActions());
    });

    connect(action, &WidgetAction::sortIndexChanged, this, [this](std::int32_t sortIndex) -> void {
        emit actionsChanged(getActions());
    });

    connect(action, &WidgetAction::stretchChanged, this, [this](std::int32_t stretch) -> void {
        emit actionsChanged(getActions());
    });

    emit actionsChanged(getActions());
}

void HorizontalGroupAction::removeAction(const WidgetAction* action)
{
    if (!_actions.contains(action))
        return;

    _actions.removeOne(action);

    if (_widgetFlagsMap.contains(action))
        _widgetFlagsMap.remove(action);

    disconnect(action, &WidgetAction::configurationFlagToggled, this, nullptr);
}

HorizontalGroupAction::WidgetFlagsMap HorizontalGroupAction::getWidgetFlagsMap()
{
    return _widgetFlagsMap;
}

HorizontalGroupAction::Widget::Widget(QWidget* parent, HorizontalGroupAction* horizontalGroupAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, horizontalGroupAction, widgetFlags),
    _horizontalGroupAction(horizontalGroupAction)
{
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    const auto updateLayout = [this, layout, horizontalGroupAction]() -> void {
        QLayoutItem* layoutItem;

        while ((layoutItem = layout->takeAt(0)) != nullptr) {
            delete layoutItem->widget();
            delete layoutItem;
        }

        for (auto action : _horizontalGroupAction->getActions()) {
            if (_horizontalGroupAction->getShowLabels() && !action->isConfigurationFlagSet(WidgetAction::ConfigurationFlag::NoLabelInGroup))
                layout->addWidget(action->createLabelWidget(this));

            if (action->isConfigurationFlagSet(WidgetAction::ConfigurationFlag::AlwaysCollapsed))
                layout->addWidget(const_cast<WidgetAction*>(action)->createCollapsedWidget(this));
            else {
                const auto widgetFlags = horizontalGroupAction->getWidgetFlagsMap()[action];

                if (action->getStretch() >= 1) {
                    if (widgetFlags >= 0)
                        layout->addWidget(const_cast<WidgetAction*>(action)->createWidget(this, widgetFlags), action->getStretch());
                    else
                        layout->addWidget(const_cast<WidgetAction*>(action)->createWidget(this), action->getStretch());
                }
                else {
                    if (widgetFlags >= 0)
                        layout->addWidget(const_cast<WidgetAction*>(action)->createWidget(this, widgetFlags));
                    else
                        layout->addWidget(const_cast<WidgetAction*>(action)->createWidget(this));
                }
                    
            }
        }
    };

    updateLayout();

    connect(horizontalGroupAction, &HorizontalGroupAction::actionsChanged, this, updateLayout);

    setLayout(layout);
}

}
