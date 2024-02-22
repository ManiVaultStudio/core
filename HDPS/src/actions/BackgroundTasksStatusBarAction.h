// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/StatusBarAction.h"
#include "actions/HorizontalGroupAction.h"
#include "actions/TaskAction.h"
#include "actions/TasksAction.h"

#include "models/TasksListModel.h"
#include "models/TasksFilterModel.h"

namespace mv::gui {

/**
 * Background tasks status bar action class
 *
 * Horizontal group action class for display of and interaction with background tasks
 *
 * Note: This action is primarily developed for internal use (not meant to be used in third-party plugins)
 * 
 * @author Thomas Kroes
 */
class BackgroundTasksStatusBarAction : public StatusBarAction
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    BackgroundTasksStatusBarAction(QObject* parent, const QString& title);

public: // Action getters

    TaskAction& getOverallBackgroundTaskAction() { return _overallBackgroundTaskAction; }
    TasksAction& getTasksActions() { return _tasksAction; }

private:
    HorizontalGroupAction   _barGroupAction;
    TaskAction              _overallBackgroundTaskAction;   /** For showing a task progress action in the status bar */
    TasksListModel          _model;
    TasksFilterModel        _filterModel;
    TasksAction             _tasksAction;
};

}
