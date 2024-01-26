// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TaskAction.h"
#include "Application.h"

namespace mv::gui {

TaskAction::TaskAction(QObject* parent, const QString& title) :
    GroupAction(parent, title),
    _progressAction(this, "Progress"),
    _killTaskAction(this, "Cancel"),
    _task(nullptr)
{
    setShowLabels(false);
    setDefaultWidgetFlags(GroupAction::Horizontal | WidgetFlag::Default);
    setConnectionPermissionsToForceNone(true);

    _progressAction.setStretch(1);

    addAction(&_progressAction);
    addAction(&_killTaskAction);

    updateActionsReadOnly();
    updateProgressActionTextFormat();

    connect(&_killTaskAction, &TriggerAction::triggered, this, [this]() -> void {
        if (_task == nullptr)
            return;

        _task->kill();
    });
}

Task* TaskAction::getTask()
{
    return _task;
}

void TaskAction::setTask(Task* task)
{
    const auto previousTask = _task;

    if (_task != nullptr) {
        disconnect(_task, &Task::nameChanged, this, nullptr);
        disconnect(_task, &Task::progressChanged, this, nullptr);
        disconnect(_task, &Task::progressTextChanged, this, nullptr);
        disconnect(_task, &Task::statusChanged, this, nullptr);
        disconnect(_task, &Task::mayKillChanged, this, nullptr);
    }

    _task = task;
    
    const auto updateKillTaskAction = [this]() -> void {
        if (_task)
            _killTaskAction.setToolTip(QString("Cancel %1").arg(_task->getName()));
    };

    if (_task) {
        updateKillTaskAction();

        connect(_task, &Task::nameChanged, this, updateKillTaskAction);

        connect(_task, &Task::progressChanged, this, [this]() -> void {
            updateProgressActionProgress();
        });

        connect(_task, &Task::progressTextChanged, this, [this]() -> void {
            updateProgressActionTextFormat();
        });

        connect(_task, &Task::statusChanged, this, &TaskAction::updateActionsReadOnly);
        connect(_task, &Task::statusChanged, this, &TaskAction::updateProgressActionRange);
        connect(_task, &Task::statusChanged, this, &TaskAction::updateProgressActionTextFormat);
        connect(_task, &Task::mayKillChanged, this, &TaskAction::updateKillTaskActionVisibility);
    }

    emit taskChanged(previousTask, _task);

    updateActionsReadOnly();
    updateProgressActionProgress();
    updateProgressActionTextFormat();
    updateProgressActionRange();
    updateKillTaskActionVisibility();
}

void TaskAction::updateActionsReadOnly()
{
    _progressAction.setEnabled(_task == nullptr ? false : _task->isRunning());
    _killTaskAction.setEnabled(_task == nullptr ? false : _task->isKillable());
}

void TaskAction::updateProgressActionProgress()
{
    if (_task)
        _progressAction.setProgress(static_cast<int>(_task->getProgress() * 100.f));
    else
        _progressAction.setProgress(0.f);
}

void TaskAction::updateProgressActionTextFormat()
{
    if (_task)
        _progressAction.setTextFormat(_task->getProgressText()); 
    else
        _progressAction.setTextFormat("No task assigned...");
}

void TaskAction::updateProgressActionRange()
{
    if (!_task)
        return;

    switch (_task->getStatus())
    {
        case Task::Status::Idle:
            _progressAction.setRange(0, 100);
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

void TaskAction::updateKillTaskActionVisibility()
{
    if (_task == nullptr)
        _killTaskAction.setVisible(false);
    else
        _killTaskAction.setVisible(_task->getMayKill());
}

ProgressAction& TaskAction::getProgressAction()
{
    return _progressAction;
}

TriggerAction& TaskAction::getKillTaskAction()
{
    return _killTaskAction;
}

}
