// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractManager.h"

#include "actions/GroupAction.h"

#ifdef _DEBUG
    #define ABSTRACT_TASK_MANAGER_VERBOSE
#endif

namespace hdps
{

class Task;

/**
 * Abstract task manager class
 *
 * Base abstract task manager class for global task management.
 *
 * @author Thomas Kroes
 */
class AbstractTaskManager : public AbstractManager
{
    Q_OBJECT

public:

    /**
     * Construct progress manager with \p parent object
     * @param parent Pointer to parent object
     */
    AbstractTaskManager(QObject* parent = nullptr) :
        AbstractManager(parent, "Task")
    {
    }

    /**
     * Get tasks group action
     * @return Reference to group action
     */
    virtual gui::GroupAction& getTasksGroupAction() = 0;

protected:

    /**
     * Adds \p task to the task manager
     * @param task Pointer to task to add
     */
    virtual void addTask(Task* task) = 0;

    /**
     * Removes \p task from the task manager
     * @param task Pointer to task to remove
     */
    virtual void removeTask(Task* task) = 0;

signals:

    friend class Task;
};

}
