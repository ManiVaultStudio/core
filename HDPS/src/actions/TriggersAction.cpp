// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TriggersAction.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

namespace hdps::gui {

TriggersAction::TriggersAction(QObject* parent, const QString& title /*= ""*/, const QVector<Trigger>& triggers /*= QVector<Trigger>()*/) :
    WidgetAction(parent, title)
{
    setText(title);
    setDefaultWidgetFlags(WidgetFlag::Default);
    setConfigurationFlag(WidgetAction::ConfigurationFlag::NoLabelInGroup);
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
    auto triggerPushButton = new QPushButton(trigger._text);

    triggerPushButton->setEnabled(trigger._enabled);
    triggerPushButton->setToolTip(trigger._tooltip);

    const auto triggerIndex = _triggers.indexOf(trigger);

    connect(triggerPushButton, &QPushButton::clicked, this, [this, triggerIndex]() {
        emit triggered(triggerIndex);
    });

    return triggerPushButton;
}

TriggersAction::Widget::Widget(QWidget* parent, TriggersAction* triggersAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, triggersAction, widgetFlags),
    _triggersAction(triggersAction)
{
    const auto applyLayout = [&](QLayout* layout) {
        layout->setContentsMargins(0, 0, 0, 0);

        setLayout(layout);
    };

    if (widgetFlags & Horizontal) {
        const auto updateLayout = [this, applyLayout]() -> void {
            auto layout = new QHBoxLayout();

            for (auto& trigger : _triggersAction->getTriggers())
                layout->addWidget(_triggersAction->createTriggerPushButton(trigger));

            applyLayout(layout);
        };

        connect(triggersAction, &TriggersAction::triggersChanged, this, updateLayout);

        updateLayout();
    }

    if (widgetFlags & Vertical) {
        const auto updateLayout = [this, applyLayout]() -> void {
            auto layout = new QVBoxLayout();

            for (auto& trigger : _triggersAction->getTriggers())
                layout->addWidget(_triggersAction->createTriggerPushButton(trigger));

            applyLayout(layout);
        };

        connect(triggersAction, &TriggersAction::triggersChanged, this, updateLayout);

        updateLayout();
    }

    const auto updatePushButton = [this](std::int32_t triggerIndex, const Trigger& trigger) {
        auto itemAt = layout()->itemAt(triggerIndex);

        if (itemAt == nullptr)
            return;

        auto pushButton = dynamic_cast<QPushButton*>(itemAt->widget());

        if (pushButton == nullptr)
            return;

        pushButton->setEnabled(trigger._enabled);
        pushButton->setText(trigger._text);
        pushButton->setToolTip(trigger._tooltip);
    };

    connect(triggersAction, &TriggersAction::triggerChanged, this, updatePushButton);
}

}
