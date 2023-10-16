// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractTaskHandler.h"
#include "TasksStatusBarAction.h"
#include "BackgroundTask.h"

#include "actions/TaskAction.h"
#include "actions/HorizontalGroupAction.h"

namespace mv {

/**
 * Background task handler class
 *
 * Interact with tasks in a main window status bar pop up window.
 *
 * @author Thomas Kroes
 */
class BackgroundTaskHandler final : public AbstractTaskHandler
{
public:

    /**
    * Construct task handler with \p parent object
    * @param parent Pointer to parent object
    */
    BackgroundTaskHandler(QObject* parent);

    /**
     * Get status bar action
     * @return Pointer to status bar widget action
     */
    gui::WidgetAction* getStatusBarAction() override { return &_statusBarAction; }

    /**
     * Get overall background task
     * @return Aggregate background task to which all other background tasks are (in)directly parented
     */
    BackgroundTask& getOverallBackgroundTask();

public: // Action getters

    gui::TaskAction& getOverallBackgroundTaskAction() { return _overallBackgroundTaskAction; }
    gui::TasksStatusBarAction& getTasksStatusBarAction() { return _tasksStatusBarAction; }

private:
    BackgroundTask              _overallBackgroundTask;         /** Aggregate background task to which all other background tasks are (in)directly parented */
    gui::TaskAction             _overallBackgroundTaskAction;   /** For showing a task progress action in the status bar */
    gui::TasksStatusBarAction   _tasksStatusBarAction;          /** Status bar action which show the number of background tasks */
    gui::HorizontalGroupAction  _statusBarAction;               /** Combines the overall background task action and the tasks status bar action */
};

}
