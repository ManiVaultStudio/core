// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractManager.h"
#include "Task.h"

#include "models/TasksListModel.h"

#ifdef _DEBUG
    #define ABSTRACT_TASK_MANAGER_VERBOSE
#endif

namespace mv
{

/**
 * Abstract task manager class
 *
 * Base abstract task manager class for global task management.
 *
 * @ingroup mv_managers
 * @author Thomas Kroes
 */
class CORE_EXPORT AbstractTaskManager : public AbstractManager
{
    Q_OBJECT

public:

    using Tasks = QVector<Task*>;

public:

    /**
     * Construct manager with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    AbstractTaskManager(QObject* parent) :
        AbstractManager(parent, "Tasks")
    {
    }

    /** Perform manager startup initialization */
    void initialize() override {}

    /**
     * Get all tasks
     * @return Vector of tasks
     */
    virtual Tasks getTasks() = 0;

    /**
     * Get tasks by handler type and \p status
     * @return Vector of tasks
     */
    template<typename TaskHandlerType>
    Tasks getTasksByHandlerTypeAndStatus(const Task::Status& status) {
        Tasks tasks = getTasks();
        Tasks tasksByHandlerType;

        std::copy_if(tasks.begin(), tasks.end(), std::back_inserter(tasksByHandlerType), [status](Task* task) {
            return dynamic_cast<TaskHandlerType*>(task->getHandler()) && task->getStatus() == status;
        });

        return tasks;
    }

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

    /**
     * Signals that \p task is added
     * @param task Task which is added
     */
    void taskAdded(Task* task);

    /**
     * Signals that \p task is about to be removed
     * @param task Task which is about to be removed
     */
    void taskAboutToBeRemoved(Task* task);

    /**
     * Signals that task with \p taskId is removed
     * @param taskId Globally unique identifier of the task which is removed
     */
    void taskRemoved(const QString& taskId);

    friend class Task;
};

}
