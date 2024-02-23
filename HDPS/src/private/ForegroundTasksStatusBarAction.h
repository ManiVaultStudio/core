// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "StatusBarAction.h"

#include <actions/TasksAction.h>

#include <models/TasksListModel.h>
#include <models/TasksFilterModel.h>

/**
 * Foreground tasks status bar action class
 *
 * Horizontal group action class for display of and interaction with foreground tasks
 *
 * @author Thomas Kroes
 */
class ForegroundTasksStatusBarAction : public StatusBarAction
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    ForegroundTasksStatusBarAction(QObject* parent, const QString& title);

private:
    mv::TasksListModel              _model;             /** Tasks list model */
    mv::TasksFilterModel            _filterModel;       /** Filter model for the tasks model */
    mv::gui::HorizontalGroupAction  _barGroupAction;    /** Bar group action */
    mv::gui::TasksAction            _tasksAction;       /** Tasks action for displaying the tasks */
};
