// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TaskAction.h"
#include "Application.h"

namespace hdps::gui {

TaskAction::TaskAction(QObject* parent, const QString& title) :
    GroupAction(parent, title),
    _progressAction(this, "Progress"),
    _killTaskAction(this, "Kill"),
    _task(nullptr)
{
    setShowLabels(false);
    setDefaultWidgetFlags(GroupAction::Horizontal | WidgetFlag::Default);
    setConnectionPermissionsToForceNone(true);

    _progressAction.setStretch(1);

    _killTaskAction.setIcon(Application::getIconFont("FontAwesome").getIcon("times"));
    _killTaskAction.setDefaultWidgetFlags(TriggerAction::Icon);

    addAction(&_progressAction);
    addAction(&_killTaskAction);

    updateActionsReadOnly();
    updateProgressActionTextFormat();
}

Task* TaskAction::getTask()
{
    return _task;
}

void TaskAction::setTask(Task* task)
{
    Q_ASSERT(task != nullptr);

    if (task == nullptr)
        return;

    if (_task != nullptr) {
        disconnect(_task, &Task::progressChanged, this, nullptr);
        disconnect(_task, &Task::statusChanged, this, nullptr);
    }

    _task = task;

    const auto updateProgressAction = [this]() -> void {
        _progressAction.setValue(static_cast<int>(_task->getProgress() * 100.f));
    };

    updateProgressAction();

    connect(_task, &Task::progressChanged, this, updateProgressAction);
    connect(_task, &Task::progressDescriptionChanged, this, &TaskAction::updateProgressActionTextFormat);
    connect(_task, &Task::statusChanged, this, &TaskAction::updateActionsReadOnly);
    connect(_task, &Task::statusChanged, this, &TaskAction::updateProgressActionRange);
    connect(_task, &Task::statusChanged, this, &TaskAction::updateProgressActionTextFormat);
}

void TaskAction::updateActionsReadOnly()
{
    _progressAction.setEnabled(_task == nullptr ? false : _task->isRunning());
    _killTaskAction.setEnabled(_task == nullptr ? false : (_task->isIdle() || _task->isRunning()));
}

void TaskAction::updateProgressActionTextFormat()
{
    if (_task == nullptr)
        _progressAction.setTextFormat("No task assigned...");
    else
        _progressAction.setTextFormat(_task->getProgressText());
}

void TaskAction::updateProgressActionRange()
{
    switch (_task->getStatus())
    {
        case Task::Status::Idle:
            break;

        case Task::Status::Running:
            _progressAction.setRange(0, 100);
            break;

        case Task::Status::RunningIndeterminate:
            _progressAction.setRange(0, 0);
            break;

        case Task::Status::Finished:
        case Task::Status::Aborted:
            break;

        default:
            break;
    }

    
}

}
