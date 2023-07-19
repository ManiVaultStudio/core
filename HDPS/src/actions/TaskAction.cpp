// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TaskAction.h"
#include "Application.h"

namespace hdps::gui {

TaskAction::TaskAction(QObject* parent, const QString& title /*= ""*/) :
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
    connect(_task, &Task::statusChanged, this, &TaskAction::updateActionsReadOnly);
}

void TaskAction::updateActionsReadOnly()
{
    _progressAction.setEnabled(_task == nullptr ? false : _task->isRunning());
    _killTaskAction.setEnabled(_task == nullptr ? false : (_task->isIdle() || _task->isRunning()));
}

}
