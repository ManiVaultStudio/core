// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ModalTaskHandler.h"
#include "ModalTask.h"

#include "CoreInterface.h"
#include "actions/TaskAction.h"

namespace hdps {

using namespace gui;

ModalTaskHandler::ModalTasksDialog ModalTaskHandler::modalTasksDialog = ModalTasksDialog();

ModalTaskHandler::ModalTaskHandler(QObject* parent /*= nullptr*/) :
    AbstractTaskHandler(parent)
{
}

void ModalTaskHandler::init()
{
}

ModalTaskHandler::ModalTasksDialog::ModalTasksDialog(QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _modalTasks(),
    _tasksGroupAction(this, "Modal Tasks")
{
    connect(&tasks(), &AbstractTaskManager::taskAdded, this, [this](Task* task) -> void {
        const auto modalTask = dynamic_cast<ModalTask*>(task);

        if (modalTask)
            _modalTasks << modalTask;

        connect(task, &Task::statusChanged, this, &ModalTasksDialog::tasksChanged);

        _tasksGroupAction.addAction(new TaskAction(this, task->getName()));

        tasksChanged();
    });

    connect(&tasks(), &AbstractTaskManager::taskAboutToBeRemoved, this, [this](Task* task) -> void {
        const auto modalTask = dynamic_cast<ModalTask*>(task);

        if (modalTask && _modalTasks.contains(modalTask))
            _modalTasks.removeOne(modalTask);

        disconnect(task, &Task::statusChanged, this, nullptr);

        for (auto action : _tasksGroupAction.getActions())
            if (task == static_cast<TaskAction*>(action)->getTask())
                _tasksGroupAction.removeAction(action);

        tasksChanged();
    });
}

void ModalTaskHandler::ModalTasksDialog::tasksChanged()
{
    ModalTasks idleOrRunningModalTasks;

    std::copy_if(idleOrRunningModalTasks.begin(), idleOrRunningModalTasks.end(), std::back_inserter(_modalTasks), [](Task* task) {
        return task->getStatus() == Task::Status::Idle || task->getStatus() == Task::Status::Running;
    });
}

}
