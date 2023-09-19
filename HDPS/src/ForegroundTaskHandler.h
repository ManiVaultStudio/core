// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractTaskHandler.h"

#include "actions/TasksAction.h"

namespace hdps {

/**
 * Foreground task handler class
 *
 * Creates the popup interface that displays running foreground tasks.
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

    /** Initializes the handler */
    void init() override;

    gui::TasksAction& getTasksAction() { return _tasksAction; }

private:
    gui::TasksAction    _tasksAction;   /** Tasks action for showing foreground tasks */
};

}
