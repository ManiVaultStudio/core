// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectSerializationTask.h"

namespace hdps {

ProjectSerializationTask::ProjectSerializationTask(QObject* parent, const QString& name, const Status& status /*= Status::Undefined*/, bool mayKill /*= false*/) :
    Task(parent, name, GuiScope::Modal, status, mayKill, nullptr),
    _mode(Mode::Load),
    _dataTask(this, ""),
    _compressionTask(this, ""),
    _workspaceTask(this, ""),
    _systemViewPluginsTask(this, ""),
    _viewPluginsTask(this, "")
{
    setStatus(Task::Status::Idle, "", true);
    setEnabled(false, true);
    setMode(_mode);

    _dataTask.setParentTask(this);
    _compressionTask.setParentTask(this);
    _workspaceTask.setParentTask(this);
    _systemViewPluginsTask.setParentTask(&_workspaceTask);
    _viewPluginsTask.setParentTask(&_workspaceTask);
}

ProjectSerializationTask::Mode ProjectSerializationTask::getMode() const
{
    return _mode;
}

void ProjectSerializationTask::setMode(const Mode& mode)
{
    switch (mode)
    {
        case Mode::Load:
        {
            _dataTask.setName("Load data");
            _compressionTask.setName("De-compress data");
            _workspaceTask.setName("Load workspace");
            _systemViewPluginsTask.setName("Load system view plugins");
            _viewPluginsTask.setName("Load view plugins");

            break;
        }

        case Mode::Save:
        {
            _dataTask.setName("Save data");
            _compressionTask.setName("Compress data");
            _workspaceTask.setName("Save workspace");
            _systemViewPluginsTask.setName("Save system view plugins");
            _viewPluginsTask.setName("Save view plugins");

            break;
        }

        default:
            break;
    }
}

ModalTask& ProjectSerializationTask::getDataTask()
{
    return _dataTask;
}

ModalTask& ProjectSerializationTask::getCompressionTask()
{
    return _compressionTask;
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

void ProjectSerializationTask::setToLoad()
{
    setMode(Mode::Load);
}

void ProjectSerializationTask::setToSave()
{
    setMode(Mode::Save);
}

}
