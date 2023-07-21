// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ModalTaskHandler.h"

#include "CoreInterface.h"
#include "actions/TaskAction.h"

namespace hdps {

using namespace gui;

QDialog ModalTaskHandler::tasksDialog = QDialog();

ModalTaskHandler::ModalTaskHandler(QObject* parent /*= nullptr*/) :
    AbstractTaskHandler(parent),
    _tasksGroupAction(this, "Tasks")
{
    connect(&tasks(), &AbstractTaskManager::taskAdded, this, [this](Task* task) -> void {
        if (!(task->isIdle() || task->isRunning()))
            return;

        const auto modelTaskHandler = dynamic_cast<ModalTaskHandler*>(task->getHandler());

        if (!modelTaskHandler)
            return;

        _tasksGroupAction.addAction(new TaskAction(this, task->getName()));

        if (_tasksGroupAction.getActions().count() == 1)
            tasksDialog.open();
    });
    
    connect(&tasks(), &AbstractTaskManager::taskAboutToBeRemoved, this, [this](Task* task) -> void {
        if (!(task->isIdle() || task->isRunning()))
            return;

        const auto modelTaskHandler = dynamic_cast<ModalTaskHandler*>(task->getHandler());

        if (!modelTaskHandler)
            return;

        for (auto action : _tasksGroupAction.getActions())
            if (task == static_cast<TaskAction*>(action)->getTask())
                _tasksGroupAction.removeAction(action);

        if (_tasksGroupAction.getActions().empty())
            tasksDialog.close();
    });
}

void ModalTaskHandler::init()
{
}

}
