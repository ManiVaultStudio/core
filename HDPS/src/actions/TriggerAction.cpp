#include "TriggerAction.h"

#include <QHBoxLayout>

namespace hdps {

namespace gui {

TriggerAction::TriggerAction(QObject* parent, const QString& title /*= ""*/) :
    WidgetAction(parent)
{
    setText(title);
    setDefaultWidgetFlags(WidgetFlag::Text);
}

void TriggerAction::setValue(const QVariant& value)
{
    // Trigger action has no value
}

QVariant TriggerAction::valueToVariant() const
{
    // Trigger action has no value
    return QVariant();
}

QVariant TriggerAction::defaultValueToVariant() const
{
    // Trigger action has no default value
    return QVariant();
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

    return new TriggerAction::PushButtonWidget(parent, this, widgetFlags);
}

}
}