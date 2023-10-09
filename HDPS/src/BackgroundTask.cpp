// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "BackgroundTask.h"
#include "BackgroundTaskHandler.h"
#include "Application.h"
#include "CoreInterface.h"

namespace hdps {

BackgroundTaskHandler* BackgroundTask::backgroundTaskHandler = nullptr;

BackgroundTask::BackgroundTask(QObject* parent, const QString& name, const Status& status /*= Status::Undefined*/, bool mayKill /*= false*/) :
    Task(parent, name, GuiScope::Background, status, mayKill, nullptr)
{
    if (core() != nullptr && core()->isInitialized())
        setParentTask(Application::current()->getTask(Application::TaskType::OverallBackground));
}

void BackgroundTask::createHandler(QObject* parent)
{
    if (BackgroundTask::backgroundTaskHandler != nullptr)
        return;

    BackgroundTask::backgroundTaskHandler = new BackgroundTaskHandler(parent);

    backgroundTaskHandler->getOverallBackgroundTaskAction().setTask(Application::current()->getTask(Application::TaskType::OverallBackground));
}

}
