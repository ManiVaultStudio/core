// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "BackgroundTask.h"

#include "BackgroundTaskHandler.h"

namespace mv {

BackgroundTaskHandler* BackgroundTask::backgroundTaskHandler = nullptr;

BackgroundTask::BackgroundTask(QObject* parent, const QString& name, bool parentToOverallBackgroundTask /*= true*/, const Status& status /*= Status::Undefined*/, bool mayKill /*= false*/) :
    Task(parent, name, GuiScopes{ GuiScope::Background }, status, mayKill, nullptr)
{
    if (parentToOverallBackgroundTask)
        setParentTask(&BackgroundTask::backgroundTaskHandler->getOverallBackgroundTask());
}

void BackgroundTask::createHandler(QObject* parent)
{
    if (BackgroundTask::backgroundTaskHandler != nullptr)
        return;

    BackgroundTask::backgroundTaskHandler = new BackgroundTaskHandler(parent);
}

BackgroundTask& BackgroundTask::getOverallBackgroundTask()
{
    return BackgroundTask::backgroundTaskHandler->getOverallBackgroundTask();
}

}
