// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/StatusBarAction.h"
#include "actions/TasksAction.h"

namespace mv::gui {

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
    HorizontalGroupAction   _barGroupAction;
    TasksAction             _tasksAction;
};

}
