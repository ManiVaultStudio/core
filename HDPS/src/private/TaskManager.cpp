// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TaskManager.h"

#include <Application.h>

#include <util/Exception.h>

#include <Task.h>

#ifdef _DEBUG
    #define TASK_MANAGER_VERBOSE
#endif

namespace hdps
{

TaskManager::TaskManager(QObject* parent /*= nullptr*/) :
    AbstractTaskManager(),
    _tasks()
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

    Application::current()->getStartupTask().setSubtaskFinished("Initializing task manager");

    AbstractTaskManager::initialize();
}

void TaskManager::reset()
{
#ifdef TASK_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    beginReset();
    {
        for (auto task : _tasks)
            removeTask(task);
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
            throw std::runtime_error("Task may not be a null pointer");

        if (!_tasks.contains(task))
            throw std::runtime_error("Task not found in manager");

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
