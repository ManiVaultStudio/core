// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ForegroundTaskHandler.h"
#include "TasksTreeModel.h"
#include "Application.h"
#include "CoreInterface.h"

using namespace hdps::gui;

namespace hdps {

ForegroundTaskHandler::ForegroundTaskHandler(QObject* parent) :
    AbstractTaskHandler(parent, nullptr),
    _tasksStatusBarAction(*tasks().getTreeModel(), this, "Foreground Tasks"),
    _statusBarAction(this, "Status Bar Group")
{
    _tasksStatusBarAction.setPopupMode(TasksStatusBarAction::PopupMode::Automatic);
    _tasksStatusBarAction.getMenu().addAction(&settings().getTasksSettingsAction().getHideForegroundTasksPopupAction());

    _statusBarAction.addAction(&_tasksStatusBarAction);

    auto& tasksFilterModel = _tasksStatusBarAction.getTasksFilterModel();

    tasksFilterModel.getTaskScopeFilterAction().setSelectedOptions({ "Foreground" });
    tasksFilterModel.getTaskStatusFilterAction().setSelectedOptions({ "Running Indeterminate", "Running", "Finished"});

    const auto hideForegroundTasksPopupChanged = [this]() -> void {
        const auto hideForegroundTasksPopup = settings().getTasksSettingsAction().getHideForegroundTasksPopupAction().isChecked();

        _tasksStatusBarAction.setPopupMode(hideForegroundTasksPopup ? TasksStatusBarAction::PopupMode::Hover : TasksStatusBarAction::PopupMode::Automatic);
        _tasksStatusBarAction.setPopupForceHidden(hideForegroundTasksPopup);
    };

    hideForegroundTasksPopupChanged();

    connect(&settings().getTasksSettingsAction().getHideForegroundTasksPopupAction(), &ToggleAction::toggled, this, hideForegroundTasksPopupChanged);
}

}
