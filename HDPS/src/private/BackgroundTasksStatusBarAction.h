// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "StatusBarAction.h"

#include <actions/HorizontalGroupAction.h>
#include <actions/TaskAction.h>
#include <actions/TasksAction.h>

#include <models/TasksListModel.h>
#include <models/TasksFilterModel.h>

/**
 * Background tasks status bar action class
 *
 * Horizontal group action class for display of and interaction with background tasks
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

    mv::gui::TaskAction& getOverallBackgroundTaskAction() { return _overallBackgroundTaskAction; }
    mv::gui::TasksAction& getTasksActions() { return _tasksAction; }

private:
    mv::TasksListModel              _model;                         /** Tasks list model */
    mv::TasksFilterModel            _filterModel;                   /** Filter model for the tasks model */
    mv::gui::HorizontalGroupAction  _barGroupAction;                /** Bar group action */
    mv::gui::TaskAction             _overallBackgroundTaskAction;   /** For showing overall background task progress */
    mv::gui::TasksAction            _tasksAction;                   /** Tasks action for displaying the tasks */
};