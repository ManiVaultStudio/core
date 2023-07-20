// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TasksAction.h"

#include <CoreInterface.h>
#include <AbstractTaskManager.h>

#include <actions/TaskAction.h>

using namespace hdps;
using namespace hdps::gui;

TasksAction::TasksAction(QObject* parent, const QString& title /*= ""*/) :
    GroupAction(parent, title)
{
    setConnectionPermissionsToForceNone();
    setConfigurationFlag(WidgetAction::ConfigurationFlag::NoLabelInGroup);
    setIcon(Application::getIconFont("FontAwesome").getIcon("tasks"));
    setDefaultWidgetFlag(GroupAction::Vertical);

    connect(&tasks(), &AbstractTaskManager::taskAdded, this, [this](Task* task) -> void {
        auto taskAction = new TaskAction(this, task->getName());

        taskAction->setTask(task);

        addAction(taskAction);
    });
}