// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ForegroundTask.h"
#include "ForegroundTaskHandler.h"
#include "Application.h"
#include "CoreInterface.h"

namespace mv {

ForegroundTaskHandler* ForegroundTask::foregroundTaskHandler = nullptr;

ForegroundTask::ForegroundTask(QObject* parent, const QString& name, const Status& status /*= Status::Undefined*/, bool mayKill /*= false*/) :
    Task(parent, name, GuiScopes{ GuiScope::Foreground }, status, mayKill, nullptr)
{
}

void ForegroundTask::createHandler(QObject* parent)
{
    if (ForegroundTask::foregroundTaskHandler != nullptr)
        return;

    ForegroundTask::foregroundTaskHandler = new ForegroundTaskHandler(parent);
}

}
