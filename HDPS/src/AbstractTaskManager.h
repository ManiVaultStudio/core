// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractManager.h"
#include "TasksTreeModel.h"
#include "TasksListModel.h"
#include "Task.h"

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
 * @author Thomas Kroes
 */
class AbstractTaskManager : public AbstractManager
{
    Q_OBJECT

public:

    using Tasks = QVector<Task*>;

public:

    /**
     * Construct progress manager with \p parent object
     * @param parent Pointer to parent object
     */
    AbstractTaskManager(QObject* parent = nullptr) :
        AbstractManager(parent, "Tasks"),
        _tasksTreeModel(nullptr),
        _tasksListModel(nullptr)
    {
    }

    /** Perform manager startup initialization */
    void initialize() override {
        _tasksTreeModel = new TasksTreeModel(this);
        _tasksListModel = new TasksListModel(this);
    }

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

public: // Model getters

    /**
     * Get tasks tree model
     * @return Reference to tasks tree model instance
     */
    TasksTreeModel* getTreeModel() {
        if (_tasksTreeModel == nullptr)
            throw std::runtime_error("Tasks tree model not initialized");

        return _tasksTreeModel;
    }

    /**
     * Get tasks list model
     * @return Pointer to tasks list model instance
     */
    TasksListModel* getListModel() {
        if (_tasksListModel == nullptr)
            throw std::runtime_error("Tasks list model not initialized");

        return _tasksListModel;
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

private:
    TasksTreeModel* _tasksTreeModel;    /** Tasks tree model instance */
    TasksListModel* _tasksListModel;    /** Tasks list model instance */

    friend class Task;
};

}
