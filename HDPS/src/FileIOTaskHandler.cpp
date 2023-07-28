// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "FileIOTaskHandler.h"
#include "FileIOTask.h"

#include "CoreInterface.h"

#include "actions/TaskAction.h"

namespace hdps {

using namespace gui;

//FileIOTaskHandler::FileIOTasksDialog FileIOTaskHandler::fileIOTasksDialog = FileIOTasksDialog();

FileIOTaskHandler::FileIOTaskHandler(QObject* parent /*= nullptr*/) :
    AbstractTaskHandler(parent)
{
}

void FileIOTaskHandler::init()
{
}

FileIOTaskHandler::FileIOTasksDialog::FileIOTasksDialog(QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _modalTasks(),
    _tasksGroupAction(this, "File IO Tasks")
{
    /*
    connect(&tasks(), &AbstractTaskManager::taskAdded, this, [this](Task* task) -> void {
        const auto fileIOTask = dynamic_cast<FileIOTask*>(task);

        if (fileIOTask)
            _modalTasks << fileIOTask;

        connect(task, &Task::statusChanged, this, &FileIOTasksDialog::tasksChanged);

        _tasksGroupAction.addAction(new TaskAction(this, task->getName()));

        tasksChanged();
    });

    connect(&tasks(), &AbstractTaskManager::taskAboutToBeRemoved, this, [this](Task* task) -> void {
        const auto fileIOTask = dynamic_cast<FileIOTask*>(task);

        if (fileIOTask && _modalTasks.contains(fileIOTask))
            _modalTasks.removeOne(fileIOTask);

        disconnect(task, &Task::statusChanged, this, nullptr);

        for (auto action : _tasksGroupAction.getActions())
            if (task == static_cast<TaskAction*>(action)->getTask())
                _tasksGroupAction.removeAction(action);

        tasksChanged();
    });
    */
}

void FileIOTaskHandler::FileIOTasksDialog::tasksChanged()
{
    /*
    ModalTasks idleOrRunningModalTasks;

    std::copy_if(idleOrRunningModalTasks.begin(), idleOrRunningModalTasks.end(), std::back_inserter(_modalTasks), [](Task* task) {
        return task->getStatus() == Task::Status::Idle || task->getStatus() == Task::Status::Running;
    });
    */
}

}
