// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ForegroundTasksStatusBarAction.h"

#ifdef _DEBUG
    #define FOREGROUND_TASKS_STATUS_BAR_ACTION_VERBOSE
#endif

#include <QDebug>

namespace mv::gui {

ForegroundTasksStatusBarAction::ForegroundTasksStatusBarAction(QObject* parent, const QString& title) :
    StatusBarAction(parent, title),
    _barGroupAction(this, "Bar group"),
    _tasksActions(this, "Tasks")
{
    setBarAction(&_barGroupAction);
    setPopupAction(&_tasksActions);

    _barGroupAction.setShowLabels(false);
    _barGroupAction.addAction(&_tasksActions);

    auto& tasksFilterModel = _tasksActions.getTasksFilterModel();

    tasksFilterModel.getTaskScopeFilterAction().setSelectedOptions({ "Foreground" });
    tasksFilterModel.getTaskStatusFilterAction().setSelectedOptions({ "Running Indeterminate", "Running", "Finished", "Aborting" });
    tasksFilterModel.getTopLevelTasksOnlyAction().setChecked(true);

    /*
    _tasksStatusBarAction.setPopupMode(TasksStatusBarAction::PopupMode::Automatic);
    _tasksStatusBarAction.getMenu().addAction(&settings().getTasksSettingsAction().getHideForegroundTasksPopupAction());

    _statusBarAction.addAction(&_tasksStatusBarAction);
    _statusBarAction.setShowLabels(false);
    */

    /*
    const auto hideForegroundTasksPopupChanged = [this]() -> void {
        const auto hideForegroundTasksPopup = settings().getTasksSettingsAction().getHideForegroundTasksPopupAction().isChecked();

        _tasksStatusBarAction.setPopupMode(hideForegroundTasksPopup ? TasksStatusBarAction::PopupMode::Hover : TasksStatusBarAction::PopupMode::Automatic);
        _tasksStatusBarAction.setPopupForceHidden(hideForegroundTasksPopup);
    };

    hideForegroundTasksPopupChanged();

    connect(&settings().getTasksSettingsAction().getHideForegroundTasksPopupAction(), &ToggleAction::toggled, this, hideForegroundTasksPopupChanged);
    */
}

}
