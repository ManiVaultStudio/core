// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <actions/GroupAction.h>

/**
 * Tasks action class
 *
 * Group action for managing tasks
 *
 * @author Thomas Kroes
 */
class TasksAction : public hdps::gui::GroupAction
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE TasksAction(QObject* parent, const QString& title = "");

    friend class AbstractActionsManager;
};

Q_DECLARE_METATYPE(TasksAction)

inline const auto tasksActionMetaTypeId = qRegisterMetaType<TasksAction*>("TasksAction");
