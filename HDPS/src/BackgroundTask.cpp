// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "BackgroundTask.h"
#include "BackgroundTaskHandler.h"
#include "Application.h"
#include "CoreInterface.h"

namespace hdps {

BackgroundTask::BackgroundTask(QObject* parent, const QString& name, Task* parentTask /*= nullptr*/, const Status& status /*= Status::Undefined*/, bool mayKill /*= false*/) :
    Task(parent, name, Scope::Background, parentTask, status, mayKill, nullptr)
{
    if (core() != nullptr && core()->isInitialized() && parentTask == nullptr)
        setParentTask(&tasks().getOverallBackgroundTask());

    setHandler(new BackgroundTaskHandler(this, this));
}

}
