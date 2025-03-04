// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "EventAction.h"
#include "Plugin.h"

namespace mv::gui {

EventAction::EventAction(QObject* parent, const QString& title) :
    VerticalGroupAction(parent, title)
{
    setConnectionPermissionsToAll(true);
}

void EventAction::triggerEvent(const QString& eventType, const QVariant& eventData)
{
    emit eventTriggered(eventType, eventData);
}

void EventAction::selfEventTriggered(const QString& eventType, const QVariant& eventData)
{
    auto publicEventAction = dynamic_cast<EventAction*>(getPublicAction());

    if (!publicEventAction)
        return;

    disconnect(publicEventAction, &EventAction::eventTriggered, this, &EventAction::triggerEvent);
    {
        publicEventAction->triggerEvent(eventType, eventData);
    }
    connect(publicEventAction, &EventAction::eventTriggered, this, &EventAction::triggerEvent);
}

void EventAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    auto publicEventAction = dynamic_cast<EventAction*>(publicAction);

    Q_ASSERT(publicEventAction);

    if (!publicEventAction)
        return;

    connect(this, &EventAction::eventTriggered, this, &EventAction::selfEventTriggered);
    connect(publicEventAction, &EventAction::eventTriggered, this, &EventAction::triggerEvent);

    WidgetAction::connectToPublicAction(publicAction, recursive);
}

void EventAction::disconnectFromPublicAction(bool recursive)
{
    if (!isConnected())
        return;

    auto publicEventAction = dynamic_cast<EventAction*>(getPublicAction());

    Q_ASSERT(publicEventAction);

    if (!publicEventAction)
        return;

    disconnect(this, &EventAction::eventTriggered, this, &EventAction::selfEventTriggered);
    disconnect(publicEventAction, &EventAction::eventTriggered, this, &EventAction::triggerEvent);

    WidgetAction::disconnectFromPublicAction(recursive);
}

}
