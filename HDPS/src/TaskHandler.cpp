// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TaskHandler.h"
#include "Task.h"

namespace hdps {

TaskHandler::TaskHandler(QObject* parent /*= nullptr*/) :
    QObject(parent),
    _task(nullptr)
{
}

Task* TaskHandler::getTask()
{
    return _task;
}

void TaskHandler::setTask(Task* task)
{
    Q_ASSERT(task != nullptr);

    if (task == nullptr)
        return;
}

}
