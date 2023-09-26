// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TasksTreeModel.h"

#include "Application.h"
#include "CoreInterface.h"

#include <util/Exception.h>

#ifdef _DEBUG
    #define TASKS_TREE_MODEL_VERBOSE
#endif

namespace hdps
{

using namespace util;

TasksTreeModel::TasksTreeModel(QObject* parent /*= nullptr*/) :
    AbstractTasksModel(parent)
{
    setColumnCount(static_cast<int>(Column::Count));

    for (auto column : columnInfo.keys())
        setHorizontalHeaderItem(static_cast<int>(column), new HeaderItem(columnInfo[column]));

    connect(&tasks(), &AbstractTaskManager::taskAdded, this, &TasksTreeModel::taskAddedToTaskManager);
    connect(&tasks(), &AbstractTaskManager::taskAboutToBeRemoved, this, &TasksTreeModel::taskAboutToBeRemovedFromTaskManager);

    for (auto task : tasks().getTasks())
        taskAddedToTaskManager(task);
}

void TasksTreeModel::taskAddedToTaskManager(Task* task)
{
    try {
        Q_ASSERT(task != nullptr);

        if (task == nullptr)
            throw std::runtime_error("Task may not be a nullptr");

        if (task->hasParentTask())
            itemFromTask(task->getParentTask())->appendRow(Row(task));
        else
            appendRow(Row(task));

        connect(task, &Task::parentTaskChanged, this, [this, task](Task* previousParentTask, Task* currentParentTask) -> void {
            try {
                auto taskItem = itemFromTask(task);

#ifdef TASKS_TREE_MODEL_VERBOSE
                qDebug() << __FUNCTION__ << task->getName() << currentParentTask->getName();
#endif

                if (previousParentTask)
                    removeRow(taskItem->row(), itemFromTask(previousParentTask)->index());
                else
                    removeRow(taskItem->row(), QModelIndex());

                if (currentParentTask)
                    itemFromTask(task->getParentTask())->appendRow(Row(task));
                else
                    appendRow(Row(task));
            }
            catch (std::exception& e)
            {
                exceptionMessageBox("Unable to re-parent task", e);
            }
            catch (...)
            {
                exceptionMessageBox("Unable to re-parent task");
            }
        });
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to add task to tasks tree model", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to add task to tasks tree model");
    }
}

void TasksTreeModel::taskAboutToBeRemovedFromTaskManager(Task* task)
{
    try {
        if (!tasks().getTasks().contains(task))
            return;

        Q_ASSERT(task != nullptr);

        if (task == nullptr)
            throw std::runtime_error("Task may not be a nullptr");

        auto taskItem = itemFromTask(task);

        if (!removeRow(taskItem->row(), taskItem->parent() ? taskItem->parent()->index() : QModelIndex()))
            throw std::runtime_error("Remove row failed");

        disconnect(task, &Task::parentTaskChanged, this, nullptr);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to remove task from tasks tree model", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to remove task from tasks tree model");
    }
}

}
