// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "BackgroundTask.h"
#include "BackgroundTaskHandler.h"

namespace hdps {

BackgroundTask::BackgroundTask(QObject* parent, const QString& name, const Status& status /*= Status::Undefined*/, bool mayKill /*= false*/) :
    Task(parent, name, status, mayKill, nullptr)
{
    setScope(Scope::Background);
    setHandler(new BackgroundTaskHandler(this, this));
}

}
