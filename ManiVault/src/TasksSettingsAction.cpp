// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TasksSettingsAction.h"
#include "Application.h"

namespace mv::gui
{

TasksSettingsAction::TasksSettingsAction(QObject* parent) :
    GlobalSettingsGroupAction(parent, "Tasks"),
    _hideForegroundTasksPopupAction(this, "Hide foreground tasks popup")
{
    setShowLabels(false);

    addAction(&_hideForegroundTasksPopupAction);
}

}
