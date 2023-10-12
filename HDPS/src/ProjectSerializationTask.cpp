// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectSerializationTask.h"
#include "Application.h"

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
    _dataTask.setParentTask(this);
    _compressionTask.setParentTask(this);
    _workspaceTask.setParentTask(this);
    _systemViewPluginsTask.setParentTask(&_workspaceTask);
    _viewPluginsTask.setParentTask(&_workspaceTask);

    setEnabled(false, true);

    if (!hasParentTask()) {
        connect(this, &Task::statusChangedToFinished, this, [this]() -> void {
            QTimer::singleShot(1500, this, [this]() -> void {
                setIdle();
            });
        });

        connect(this, &Task::statusChangedToIdle, this, [this]() -> void {
            setEnabled(false, true);
        });
    }
}

ProjectSerializationTask::Mode ProjectSerializationTask::getMode() const
{
    return _mode;
}

void ProjectSerializationTask::setMode(const Mode& mode, const QString& projectFilePath)
{
    setStatus(Task::Status::Idle);
    setEnabled(true, true);
    reset(true);

    switch (mode)
    {
        case Mode::Load:
        {
            setDescription("Loading ManiVault project from " + projectFilePath);
            setIcon(Application::getIconFont("FontAwesome").getIcon("folder-open"));

            _dataTask.setName("Load data");
            _compressionTask.setName("De-compress data");
            _workspaceTask.setName("Load workspace");
            _systemViewPluginsTask.setName("Load system view plugins");
            _viewPluginsTask.setName("Load view plugins");

            break;
        }

        case Mode::Save:
        {
            setDescription("Saving ManiVault project to " + projectFilePath);
            setIcon(Application::getIconFont("FontAwesome").getIcon("file-archive"));

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

void ProjectSerializationTask::startLoad(const QString& projectFilePath)
{
    setMode(Mode::Load, projectFilePath);
}

void ProjectSerializationTask::startSave(const QString& projectFilePath)
{
    setMode(Mode::Save, projectFilePath);
}

}