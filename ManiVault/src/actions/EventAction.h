// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "VerticalGroupAction.h"
#include "TriggerAction.h"

namespace mv::gui {

/**
 * Event action class
 *
 * For emitting generic events
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT EventAction : public VerticalGroupAction
{
    Q_OBJECT

protected: // Linking

    /**
     * Connect this action to a public action
     * @param publicAction Pointer to public action to connect to
     * @param recursive Whether to also connect descendant child actions
     */
    void connectToPublicAction(WidgetAction* publicAction, bool recursive) override;

    /**
     * Disconnect this action from its public action
     * @param recursive Whether to also disconnect descendant child actions
     */
    void disconnectFromPublicAction(bool recursive) override;

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE EventAction(QObject* parent, const QString& title);

    /**
     * Triggers an event of \p eventType with \p eventData
     * @param eventType The type of event
     * @param eventData Event data
     */
    void triggerEvent(const QString& eventType, const QVariant& eventData);

private:

    /**
     * Invoked when this event action is triggered (needed to prevent circular calls of trigger())
     * @param eventType The type of event
     * @param eventData Event data
     */
    void selfEventTriggered(const QString& eventType, const QVariant& eventData);

signals:

    /**
     * Signals that an event of \p eventType with \p eventData has been triggered
     * @param eventType The type of event
     * @param eventData Event data
     */
    void eventTriggered(const QString& eventType, const QVariant& eventData);

private:

    TriggerAction   _testAction;

    friend class AbstractActionsManager;
};

}

Q_DECLARE_METATYPE(mv::gui::EventAction)

inline const auto eventActionMetaTypeId = qRegisterMetaType<mv::gui::EventAction*>("mv::gui::EventAction");
