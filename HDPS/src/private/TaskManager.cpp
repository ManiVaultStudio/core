// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TaskManager.h"

#ifdef _DEBUG
    #define TASK_MANAGER_VERBOSE
#endif

namespace hdps
{

TaskManager::TaskManager(QObject* parent /*= nullptr*/) :
    AbstractTaskManager()
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
}

void TaskManager::reset()
{
#ifdef TASK_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    beginReset();
    {
    }
    endReset();
}

void TaskManager::addTask(Task* task)
{

}

void TaskManager::removeTask(Task* task)
{

}

}
