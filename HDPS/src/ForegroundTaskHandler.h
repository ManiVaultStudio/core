// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractTaskHandler.h"
#include "TasksStatusBarAction.h"

#include "actions/HorizontalGroupAction.h"

namespace mv {

/**
 * Foreground task handler class
 *
 * Contains a bespoke tool button for the application that displays running foreground tasks in a popup.
 *
 * @author Thomas Kroes
 */
class ForegroundTaskHandler final : public AbstractTaskHandler
{
public:

    /**
    * Construct task handler with \p parent object
    * @param parent Pointer to parent object
    */
    ForegroundTaskHandler(QObject* parent);

    /**
     * Get status bar action
     * @return Pointer to status bar widget action
     */
    gui::WidgetAction* getStatusBarAction() override { return &_statusBarAction; }

public: // Action getters

    gui::TasksStatusBarAction& getTasksStatusBarAction() { return _tasksStatusBarAction; }

private:
    gui::TasksStatusBarAction   _tasksStatusBarAction;
    gui::HorizontalGroupAction  _statusBarAction;
};

}
