// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TaskManager.h"

#include <util/Exception.h>

#include <Task.h>

#include <QListIterator>

#ifdef _DEBUG
    //#define TASK_MANAGER_VERBOSE
#endif

namespace mv
{

TaskManager::TaskManager(QObject* parent) :
    AbstractTaskManager(parent)
{
}

TaskManager::~TaskManager()
{
    reset();
}

void TaskManager::initialize()
{
#ifdef TASK_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    AbstractTaskManager::initialize();

    if (isInitialized())
        return;

    beginInitialization();
    {
    }
    endInitialization();
}

void TaskManager::reset()
{
#ifdef TASK_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    beginReset();
    {
        if (!isCoreDestroyed()) {
            QListIterator<Task*> it(_tasks);

            it.toBack();

            while (it.hasPrevious())
                removeTask(it.previous());
        }
    }
    endReset();
}

AbstractTaskManager::Tasks TaskManager::getTasks()
{
    return _tasks;
}

void TaskManager::addTask(Task* task)
{
    try
    {
        Q_ASSERT(task != nullptr);

        if (task == nullptr)
            throw std::runtime_error("Task may not be a null pointer");

#ifdef TASK_MANAGER_VERBOSE
        qDebug() << __FUNCTION__ << task->getName();
#endif

        _tasks << task;

        emit taskAdded(task);
    }
    catch (std::exception& e)
    {
        util::exceptionMessageBox("Unable to add task to the task manager", e);
    }
    catch (...)
    {
        util::exceptionMessageBox("Unable to add task to the task manager");
    }
}

void TaskManager::removeTask(Task* task)
{
    try
    {
        Q_ASSERT(task != nullptr);

        if (task == nullptr)
        {
#ifdef TASK_MANAGER_VERBOSE
            qDebug() << "Warning: Cannot remove task, task is nullptr";
#endif
            return;
        }

        if (!_tasks.contains(task))
        {
#ifdef TASK_MANAGER_VERBOSE
            qDebug() << "Warning: Cannot remove task, TaskManager does not know about it";
#endif
            return;
        }

#ifdef TASK_MANAGER_VERBOSE
        qDebug() << __FUNCTION__ << task->getName();
#endif

        const auto taskId = task->getId();

        emit taskAboutToBeRemoved(task);
        {
            _tasks.removeOne(task);
        }
        emit taskRemoved(taskId);
    }
    catch (std::exception& e)
    {
        util::exceptionMessageBox("Unable to remove task from the task manager", e);
    }
    catch (...)
    {
        util::exceptionMessageBox("Unable to remove task from the task manager");
    }
}

}
