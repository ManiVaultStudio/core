// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "BackgroundTaskHandler.h"
#include "TasksListModel.h"
#include "Application.h"
#include "CoreInterface.h"

using namespace mv::gui;

namespace mv {

BackgroundTaskHandler::BackgroundTaskHandler(QObject* parent) :
    AbstractTaskHandler(parent, nullptr),
    _overallBackgroundTaskAction(this, "Overall Background Task"),
    _tasksStatusBarAction(*tasks().getListModel(), this, "Background tasks", Application::getIconFont("FontAwesome").getIcon("window-maximize"), TasksStatusBarAction::PopupMode::Hover, Task::GuiScope::Background),
    _statusBarAction(this, "Status Bar Group")
{
    _overallBackgroundTaskAction.setStretch(1);

    _tasksStatusBarAction.setPopupMode(TasksStatusBarAction::PopupMode::Hover);
    _tasksStatusBarAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::NoLabelInGroup);

    _statusBarAction.setIcon(Application::getIconFont("FontAwesome").getIcon("search"));
    _statusBarAction.setShowLabels(false);

    _statusBarAction.addAction(&_overallBackgroundTaskAction);
    _statusBarAction.addAction(&_tasksStatusBarAction);

    auto& tasksFilterModel = _tasksStatusBarAction.getTasksFilterModel();

    /*
    tasksFilterModel.getTaskScopeFilterAction().setSelectedOptions({ "Background" });
    tasksFilterModel.getTaskStatusFilterAction().setSelectedOptions({ "Running Indeterminate", "Running", "Finished", "Aborting" });
    tasksFilterModel.getParentTaskFilterAction().setString(Application::current()->getTask(Application::TaskType::OverallBackground)->getId());

    const auto overallBackgroundTaskTextFormatter = [this](Task& task) -> QString {
        const auto numberOfChildTasks = task.getChildTasksForGuiScopesAndStatuses(false, true, { Task::GuiScope::Background }, { Task::Status::Running, Task::Status::RunningIndeterminate }).count();

        switch (task.getStatus())
        {
            case Task::Status::Undefined:
            case Task::Status::Idle:
                return "No tasks running in the background";

            case Task::Status::Running:
            case Task::Status::RunningIndeterminate:
                return QString("%1 task%2 %3 running in the background %4%").arg(QString::number(numberOfChildTasks), numberOfChildTasks == 1 ? "" : "s", numberOfChildTasks == 1 ? "is" : "are", QString::number(task.getProgress() * 100.f, 'f', 1));

            case Task::Status::Finished:
                return QString("All background tasks have finished");

            case Task::Status::AboutToBeAborted:
            case Task::Status::Aborting:
            case Task::Status::Aborted:
                return {};

            default:
                break;
        }

        return {};
    };

    Application::current()->getTask(Application::TaskType::OverallBackground)->setProgressTextFormatter(overallBackgroundTaskTextFormatter);
    */
}

}
