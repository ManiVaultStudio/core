// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "BackgroundTaskHandler.h"
#include "Application.h"

using namespace hdps::gui;

namespace hdps {

BackgroundTaskHandler::BackgroundTaskHandler(QObject* parent) :
    AbstractTaskHandler(parent, nullptr),
    _tasksListModel(this),
    _overallBackgroundTaskAction(this, "Overall Background Task"),
    _tasksStatusBarAction(_tasksListModel, this, "Tasks Status Bar"),
    _statusBarAction(this, "Status Bar Group")
{
    _overallBackgroundTaskAction.setStretch(1);

    _tasksStatusBarAction.setPopupMode(TasksStatusBarAction::PopupMode::Hover);
    _tasksStatusBarAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::NoLabelInGroup);

    _statusBarAction.setIcon(Application::getIconFont("FontAwesome").getIcon("search"));

    _statusBarAction.addAction(&_overallBackgroundTaskAction);
    _statusBarAction.addAction(&_tasksStatusBarAction);

    auto& tasksFilterModel = _tasksStatusBarAction.getTasksFilterModel();

    tasksFilterModel.getTaskScopeFilterAction().setSelectedOptions({ "Background" });
    tasksFilterModel.getTaskStatusFilterAction().setSelectedOptions({ "Running Indeterminate", "Running", "Finished" });
    tasksFilterModel.getParentTaskFilterAction().setString(Application::current()->getTask(Application::TaskType::OverallBackground)->getId());
}

}
