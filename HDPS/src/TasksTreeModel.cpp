// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TasksTreeModel.h"

#include "Application.h"
#include "CoreInterface.h"

#include <util/Exception.h>

#ifdef _DEBUG
    #define TASKS_MODEL_VERBOSE
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

        if (task->hasParentTask()) {
            const auto matches = match(index(0, static_cast<int>(Column::ID)), Qt::EditRole, task->getParentTask()->getId(), 1, Qt::MatchExactly | Qt::MatchRecursive);

            if (matches.isEmpty())
                throw std::runtime_error(QString("%1 not found").arg(task->getParentTask()->getName()).toStdString());

            auto parentItem = itemFromIndex(matches.first().siblingAtColumn(static_cast<int>(Column::ExpandCollapse)));

            Q_ASSERT(parentItem != nullptr);

            if (parentItem == nullptr)
                throw std::runtime_error("Parent standard item may not be a nullptr");

            parentItem->appendRow(Row(task));
        }
        else {
            appendRow(Row(task));
        }
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

        const auto matches = match(index(0, static_cast<int>(Column::ID)), Qt::EditRole, task->getId(), -1, Qt::MatchExactly | Qt::MatchRecursive);

        if (matches.empty())
            throw std::runtime_error(QString("%1 not found").arg(task->getName()).toStdString());

        if (!removeRow(matches.first().row()))
            throw std::runtime_error("Remove row failed");
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
