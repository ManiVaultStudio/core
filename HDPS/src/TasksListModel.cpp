// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TasksListModel.h"

#include "Application.h"
#include "CoreInterface.h"

#include <util/Exception.h>

#ifdef _DEBUG
    #define TASKS_MODEL_VERBOSE
#endif

namespace hdps
{

using namespace util;

TasksListModel::TasksListModel(QObject* parent /*= nullptr*/) :
    AbstractTasksModel(parent)
{
    setColumnCount(static_cast<int>(Column::Count));

    for (auto column : columnInfo.keys())
        setHorizontalHeaderItem(static_cast<int>(column), new HeaderItem(columnInfo[column]));

    connect(&tasks(), &AbstractTaskManager::taskAdded, this, &TasksListModel::taskAddedToTaskManager);
    connect(&tasks(), &AbstractTaskManager::taskAboutToBeRemoved, this, &TasksListModel::taskAboutToBeRemovedFromTaskManager);

    for (auto task : tasks().getTasks())
        taskAddedToTaskManager(task);
}

void TasksListModel::taskAddedToTaskManager(Task* task)
{
    try {
        Q_ASSERT(task != nullptr);

        if (task == nullptr)
            throw std::runtime_error("Task may not be a nullptr");

        appendRow(Row(task));
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to add task to tasks list model", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to add task to tasks list model");
    }
}

void TasksListModel::taskAboutToBeRemovedFromTaskManager(Task* task)
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
        exceptionMessageBox("Unable to remove task from tasks list model", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to remove task from tasks list model");
    }
}

}
