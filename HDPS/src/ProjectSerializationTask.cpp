// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectSerializationTask.h"

namespace hdps {

ProjectSerializationTask::ProjectSerializationTask(QObject* parent, const QString& name, const Status& status /*= Status::Undefined*/, bool mayKill /*= false*/) :
    Task(parent, name, GuiScope::Modal, status, mayKill, nullptr),
    _dataTask(this, "Load data"),
    _workspaceTask(this, "Load workspace"),
    _systemViewPluginsTask(this, "Load system view plugins"),
    _viewPluginsTask(this, "Load view plugins")
{
    setMayKill(false, true);

    _dataTask.setParentTask(this);
    _workspaceTask.setParentTask(this);
    _systemViewPluginsTask.setParentTask(&_workspaceTask);
    _viewPluginsTask.setParentTask(&_workspaceTask);
}

ModalTask& ProjectSerializationTask::getDataTask()
{
    return _dataTask;
}

ModalTask& ProjectSerializationTask::getWorkspaceTask()
{
    return _workspaceTask;
}

ModalTask& ProjectSerializationTask::getSystemViewPluginsTask()
{
    return _systemViewPluginsTask;
}

ModalTask& ProjectSerializationTask::getViewPluginsTask()
{
    return _viewPluginsTask;
}

}
