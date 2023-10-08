// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "GlobalSettingsGroupAction.h"

#include "actions/ToggleAction.h"

namespace hdps
{

/**
 * Tasks global settings action class
 *
 * Action class which groups all tasks-related global settings
 *
 * @author Thomas Kroes
 */
class TasksSettingsAction final : public GlobalSettingsGroupAction
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     */
    TasksSettingsAction(QObject* parent);

public: // Action getters

    gui::ToggleAction& getHideForegroundTasksPopupAction() { return _hideForegroundTasksPopupAction; }

private:
    gui::ToggleAction   _hideForegroundTasksPopupAction;     /** Toggle action for hiding the foreground tasks popup window */
};

}
