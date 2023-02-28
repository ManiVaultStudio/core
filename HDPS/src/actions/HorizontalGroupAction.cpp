#include "HorizontalGroupAction.h"

#include <QHBoxLayout>

namespace hdps::gui {

HorizontalGroupAction::HorizontalGroupAction(QObject* parent, const QString& title, const bool& expanded /*= false*/) :
    GroupAction(parent, title, expanded)
{
}

QString HorizontalGroupAction::getTypeString() const
{
    return "HorizontalGroup";
}

HorizontalGroupAction::Widget::Widget(QWidget* parent, HorizontalGroupAction* horizontalGroupAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, horizontalGroupAction, widgetFlags),
    _horizontalGroupAction(horizontalGroupAction)
{
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    const auto updateLayout = [&]() -> void {
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
            else
                layout->addWidget(const_cast<WidgetAction*>(action)->createWidget(this), action->getStretch());
        }
    };

    updateLayout();

    connect(horizontalGroupAction, &HorizontalGroupAction::actionsChanged, this, updateLayout);

    setLayout(layout);
}

}
