// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ApplicationStartupTask.h"
#include "Application.h"
#include "CoreInterface.h"
#include "ModalTask.h"
#include "ModalTaskHandler.h"

#include <QFileInfo>

namespace mv {

ApplicationStartupTask::ApplicationStartupTask(QObject* parent, const QString& name, const Status& status /*= Status::Undefined*/, bool mayKill /*= false*/) :
    Task(parent, name, GuiScopes{ GuiScope::None }, status, mayKill, nullptr),
    _loadCoreTask(this, "Load core"),
    _loadCoreManagersTask(this, "Load core managers"),
    _loadGuiTask(this, "Load GUI"),
    _loadProjectTask(this, "Load project")
{
    _loadCoreTask.setParentTask(this);
    _loadCoreManagersTask.setParentTask(&_loadCoreTask);
    _loadGuiTask.setParentTask(this);
    _loadProjectTask.setParentTask(this);

    _loadProjectTask.setEnabled(false, true);

    setStatus(Task::Status::Idle, true);

    // Call QCoreApplication::processEvents() on each task update since we live before Qt's main event loop
    _loadCoreManagersTask.setAlwaysProcessEvents(true);
    _loadGuiTask.setAlwaysProcessEvents(true);

    connect(this, &Task::statusChangedToFinished, this, [this]() -> void {
        if (Application::current()->shouldOpenProjectAtStartup())
            setProgressDescription("Loaded " + QFileInfo(Application::current()->getStartupProjectUrl()).fileName());
        else
            setProgressDescription("ManiVault Studio loaded...");

        QTimer::singleShot(2500, this, [this]() -> void {
            setIdle();

            ModalTask::getGlobalHandler()->setEnabled(true);
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
