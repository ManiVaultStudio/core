// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ApplicationStartupTask.h"
#include "Application.h"
#include "CoreInterface.h"

namespace hdps {

ApplicationStartupTask::ApplicationStartupTask(QObject* parent, const QString& name, const Status& status /*= Status::Undefined*/, bool mayKill /*= false*/) :
    Task(parent, name, GuiScope::None, status, mayKill, nullptr),
    _loadCoreTask(this, "Load core"),
    _loadCoreManagersTask(this, "Load core managers"),
    _loadGuiTask(this, "Load GUI"),
    _loadProjectTask(this, "Load project")
{
    _loadCoreTask.setParentTask(this);
    _loadCoreManagersTask.setParentTask(&_loadCoreTask);
    _loadGuiTask.setParentTask(this);
    _loadProjectTask.setParentTask(this);

    _loadProjectTask.setEnabled(false);

    setStatus(Task::Status::Idle);

    connect(this, &Task::statusChangedToFinished, this, [this]() -> void {
        QTimer::singleShot(1500, this, [this]() -> void {
            setIdle();
            });
    });

    connect(this, &Task::statusChangedToIdle, this, [this]() -> void {
        setEnabled(false, true);
    });
}

Task& ApplicationStartupTask::getLoadCoreTask()
{
    return _loadCoreTask;
}

Task& ApplicationStartupTask::getLoadCoreManagersTask()
{
    return _loadCoreManagersTask;
}

Task& ApplicationStartupTask::getLoadGuiTask()
{
    return _loadGuiTask;
}

ProjectSerializationTask& ApplicationStartupTask::getLoadProjectTask()
{
    return _loadProjectTask;
}

}
