#include "TriggerAction.h"

#include <QHBoxLayout>
#include <QMenu>

namespace hdps::gui {

TriggerAction::TriggerAction(QObject* parent, const QString& title) :
    WidgetAction(parent, title)
{
    setDefaultWidgetFlags(WidgetFlag::Text);
    setConfigurationFlag(WidgetAction::ConfigurationFlag::NoLabelInGroup);
}

void TriggerAction::selfTriggered()
{
    auto publicTriggerAction = dynamic_cast<TriggerAction*>(getPublicAction());

    if (publicTriggerAction == nullptr)
        return;

    disconnect(publicTriggerAction, &TriggerAction::triggered, this, &TriggerAction::trigger);
    {
        publicTriggerAction->trigger();
    }
    connect(publicTriggerAction, &TriggerAction::triggered, this, &TriggerAction::trigger);
}

TriggerAction::PushButtonWidget::PushButtonWidget(QWidget* parent, TriggerAction* triggerAction, const std::int32_t& widgetFlags) :
    QPushButton(parent),
    _triggersAction(triggerAction)
{
    connect(this, &QPushButton::clicked, this, [this, triggerAction]() {
        triggerAction->trigger();
    });

    const auto update = [this, triggerAction, widgetFlags]() -> void {
        QSignalBlocker blocker(this);

        setEnabled(triggerAction->isEnabled());

        if (widgetFlags & WidgetFlag::Text)
            setText(triggerAction->text());

        if (widgetFlags & WidgetFlag::Icon) {
            setIcon(triggerAction->icon());

            if ((widgetFlags & WidgetFlag::Text) == 0)
                setProperty("class", "square-button");
        }

        setToolTip(triggerAction->toolTip());
        setVisible(triggerAction->isVisible());
    };

    connect(triggerAction, &QAction::changed, this, [this, update]() {
        update();
    });

    update();
}

QWidget* TriggerAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    if (dynamic_cast<QMenu*>(parent))
        return QWidgetAction::createWidget(parent);

    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(new TriggerAction::PushButtonWidget(parent, this, widgetFlags));

    widget->setLayout(layout);

    return widget;
}

void TriggerAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    auto publicTriggerAction = dynamic_cast<TriggerAction*>(publicAction);

    Q_ASSERT(publicTriggerAction != nullptr);

    if (publicTriggerAction == nullptr)
        return;

    connect(this, &TriggerAction::triggered, this, &TriggerAction::selfTriggered);
    connect(publicTriggerAction, &TriggerAction::triggered, this, &TriggerAction::trigger);

    WidgetAction::connectToPublicAction(publicAction, recursive);
}

void TriggerAction::disconnectFromPublicAction(bool recursive)
{
    if (!isConnected())
        return;

    auto publicTriggerAction = dynamic_cast<TriggerAction*>(getPublicAction());

    Q_ASSERT(publicTriggerAction != nullptr);

    if (publicTriggerAction == nullptr)
        return;

    disconnect(this, &TriggerAction::triggered, this, &TriggerAction::selfTriggered);
    disconnect(publicTriggerAction, &TriggerAction::triggered, this, &TriggerAction::trigger);

    WidgetAction::disconnectFromPublicAction(recursive);
}

}
