// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TasksListModel.h"

#include "Application.h"
#include "CoreInterface.h"

#include <util/Exception.h>

#ifdef _DEBUG
    #define TASKS_LIST_MODEL_VERBOSE
#endif

namespace mv
{

using namespace util;

TasksListModel::TasksListModel(QObject* parent /*= nullptr*/) :
    AbstractTasksModel(parent)
{
    setColumnCount(static_cast<int>(Column::Count));

    for (auto column : columnInfo.keys())
        setHorizontalHeaderItem(static_cast<int>(column), new HeaderItem(columnInfo[column]));

    for (auto task : tasks().getTasks())
        addTask(task);
}

QPointer<Task> TasksListModel::getTask(const std::int32_t& rowIndex) const
{
    auto taskItem = dynamic_cast<AbstractTasksModel::Item*>(itemFromIndex(index(rowIndex, 0)));

    if (!taskItem)
        return nullptr;

    return taskItem->getTask();
}

void TasksListModel::addTask(Task* task)
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

}
