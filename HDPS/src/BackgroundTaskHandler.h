// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractTaskHandler.h"
#include "BackgroundTask.h"

namespace mv {

namespace gui {
    class BackgroundTasksStatusBarAction;
}

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
    gui::WidgetAction* getStatusBarAction() override;

    /**
     * Get overall background task
     * @return Aggregate background task to which all other background tasks are (in)directly parented
     */
    BackgroundTask& getOverallBackgroundTask();

private:
    BackgroundTask                          _overallBackgroundTask;     /** Aggregate background task to which all other background tasks are (in)directly parented */
    gui::BackgroundTasksStatusBarAction*    _statusBarAction;           /** Lazily initialized status bar action for the background tasks */
};

}
