#include "TriggersAction.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

namespace hdps {

namespace gui {

TriggersAction::TriggersAction(QObject* parent, const QString& title /*= ""*/, const QVector<Trigger>& triggers /*= QVector<Trigger>()*/) :
    WidgetAction(parent)
{
    setText(title);
    setDefaultWidgetFlags(WidgetFlag::Default);
    setTriggers(triggers);
}

const QVector<TriggersAction::Trigger>& TriggersAction::getTriggers() const
{
    return _triggers;
}

void TriggersAction::setTriggers(const QVector<Trigger>& triggers)
{
    _triggers = triggers;

    emit triggersChanged(_triggers);
}

void TriggersAction::setTriggerText(std::int32_t triggerIndex, const QString& text)
{
    if (triggerIndex >= _triggers.count())
        return;

    _triggers[triggerIndex]._text = text;

    emit triggerChanged(triggerIndex, _triggers[triggerIndex]);
}

void TriggersAction::setTriggerTooltip(std::int32_t triggerIndex, const QString& tooltip)
{
    if (triggerIndex >= _triggers.count())
        return;

    _triggers[triggerIndex]._tooltip = tooltip;

    emit triggerChanged(triggerIndex, _triggers[triggerIndex]);
}

void TriggersAction::setTriggerEnabled(std::int32_t triggerIndex, const bool& enabled)
{
    if (triggerIndex >= _triggers.count())
        return;

    _triggers[triggerIndex]._enabled = enabled;

    emit triggerChanged(triggerIndex, _triggers[triggerIndex]);
}

QPushButton* TriggersAction::createTriggerPushButton(const Trigger& trigger)
{
    // Create push button for the trigger
    auto triggerPushButton = new QPushButton(trigger._text);

    // Set tooltip
    triggerPushButton->setToolTip(trigger._tooltip);

    // Get trigger index
    const auto triggerIndex = _triggers.indexOf(trigger);

    // And relay the click event
    connect(triggerPushButton, &QPushButton::clicked, this, [this, triggerIndex]() {
        emit triggered(triggerIndex);
    });

    return triggerPushButton;
}

TriggersAction::Widget::Widget(QWidget* parent, TriggersAction* triggersAction, const std::int32_t& widgetFlags) :
    QWidget(parent),
    _triggersAction(triggersAction)
{
    // Apply layout to widget
    const auto applyLayout = [this](QLayout* layout) {
        layout->setMargin(0);
        setLayout(layout);
    };

    // Horizontal trigger buttons layout
    if (widgetFlags & Horizontal) {

        // Update horizontal layout
        const auto updateLayout = [this, applyLayout]() -> void {
            auto layout = new QHBoxLayout();

            // Add trigger push button for each trigger
            for (auto& trigger : _triggersAction->getTriggers())
                layout->addWidget(_triggersAction->createTriggerPushButton(trigger));

            // Apply the layout
            applyLayout(layout);
        };

        // Update horizontal layout when the triggers change
        connect(triggersAction, &TriggersAction::triggersChanged, this, updateLayout);

        // Perform an initial update of the layout
        updateLayout();
    }

    // Vertical trigger buttons layout
    if (widgetFlags & Vertical) {

        // Update vertical layout
        const auto updateLayout = [this, applyLayout]() -> void {
            auto layout = new QVBoxLayout();

            // Add trigger push button for each trigger
            for (auto& trigger : _triggersAction->getTriggers())
                layout->addWidget(_triggersAction->createTriggerPushButton(trigger));

            // Apply the layout
            applyLayout(layout);
        };

        // Update vertical layout when the triggers change
        connect(triggersAction, &TriggersAction::triggersChanged, this, updateLayout);

        // Perform an initial update of the layout
        updateLayout();
    }

    // Update push button when a trigger changed
    connect(triggersAction, &TriggersAction::triggerChanged, this, [this](std::int32_t triggerIndex, const Trigger& trigger) {

        // Get layout item at trigger index
        auto itemAt = layout()->itemAt(triggerIndex);

        if (itemAt == nullptr)
            return;

        // Get push button widget at trigger index
        auto pushButton = dynamic_cast<QPushButton*>(itemAt->widget());

        if (pushButton == nullptr)
            return;

        pushButton->setEnabled(trigger._enabled);
        pushButton->setText(trigger._text);
        pushButton->setToolTip(trigger._tooltip);
    });
}

}
}
