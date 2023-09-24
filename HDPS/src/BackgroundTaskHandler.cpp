// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "BackgroundTaskHandler.h"

namespace hdps {

BackgroundTaskHandler::BackgroundTaskHandler(QObject* parent, Task* task) :
    AbstractTaskHandler(parent, task)
{
}

void BackgroundTaskHandler::init()
{
}

}

//connect(&settings().getTasksSettingsAction().getHideForegroundTasksPopupAction(), &ToggleAction::toggled, this, [this](bool toggled) -> void {
//    if (toggled) {
//        close();
//    }
//    else {
//        if (_tasksAction.getTasksFilterModel().rowCount() >= 1 && isHidden())
//            show();
//    }
//    });


//tasksFilterModel.getTaskStatusFilterAction().setSelectedOptions({ "Running", "Running Indeterminate", "Finished" });
//tasksFilterModel.getTaskScopeFilterAction().setSelectedOptions({ "Foreground" });