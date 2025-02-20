// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectSerializationTask.h"
#include "Application.h"

#include <QFileInfo>

using namespace mv::util;

namespace mv {

ProjectSerializationTask::ProjectSerializationTask(QObject* parent, const QString& name, const Status& status /*= Status::Undefined*/, bool mayKill /*= false*/) :
    Task(parent, name, { GuiScope::Modal }, status, mayKill, nullptr),
    _mode(Mode::Load),
    _dataTask(this, "Data serialization"),
    _compressionTask(this, "Compression serialization"),
    _workspaceTask(this, "Workspace serialization"),
    _systemViewPluginsTask(this, "System view plugins serialization"),
    _viewPluginsTask(this, "View plugins serialization")
{
    _dataTask.setParentTask(this);
    _compressionTask.setParentTask(this);
    _workspaceTask.setParentTask(this);
    _systemViewPluginsTask.setParentTask(&_workspaceTask);
    _viewPluginsTask.setParentTask(&_workspaceTask);

    if (!hasParentTask()) {
        connect(this, &Task::statusChangedToFinished, this, [this]() -> void {
            QTimer::singleShot(1500, this, [this]() -> void {
                setIdle();
                setEnabled(false, true);
            });
        });
    }
}

ProjectSerializationTask::Mode ProjectSerializationTask::getMode() const
{
    return _mode;
}

void ProjectSerializationTask::setMode(const Mode& mode, const QString& projectFilePath)
{
    setStatus(Task::Status::Idle, true);
    setEnabled(true, true);
    reset(true);

    const auto projectFileName = QFileInfo(projectFilePath).fileName();

    switch (mode)
    {
        case Mode::Load:
        {
            setName("Load " + projectFileName);
            setDescription("Loading ManiVault project from " + projectFilePath);
            setIcon(StyledIcon("folder-open"));

            _dataTask.setEnabled(true);

            _dataTask.setName("Load data");
            _compressionTask.setName("De-compress data");
            _workspaceTask.setName("Load workspace");
            _systemViewPluginsTask.setName("Load system view plugins");
            _viewPluginsTask.setName("Load view plugins");

            break;
        }

        case Mode::Save:
        {
            setName("Save " + projectFileName);
            setDescription("Saving ManiVault project to " + projectFilePath);
            setIcon(StyledIcon("file-archive"));

            _dataTask.setEnabled(false);

            _dataTask.setName("Save data");
            _compressionTask.setName("Compress data");
            _workspaceTask.setName("Save workspace");
            _systemViewPluginsTask.setName("Save system view plugins");
            _viewPluginsTask.setName("Save view plugins");

            break;
        }
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
