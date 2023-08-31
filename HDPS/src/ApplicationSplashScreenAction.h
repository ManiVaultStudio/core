// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/AbstractSplashScreenAction.h"

//#include "TasksModel.h"
//#include "TasksFilterModel.h"

namespace hdps::gui {

/**
 * Application splash screen action class
 *
 * Action class for configuring and displaying an application splash screen.
 *
 * @author Thomas Kroes
 */
class ApplicationSplashScreenAction final : public AbstractSplashScreenAction
{

public:

    /**
     * Construct with \p parent object
     * @param parent Pointer to parent object
     */
    ApplicationSplashScreenAction(QObject* parent);

    /** Get an instance of a splash screen dialog */
    SplashScreenDialog* getSplashScreenDialog() override;

    /**
     * Get tasks model
     * @return Reference to the task model
     */
    //TasksModel& getTasksModel();

    /**
     * Get tasks filter model
     * @return Reference to the task filter model
     */
    //TasksFilterModel& getTasksFilterModel();

private:
    //TasksModel          _tasksModel;            /** Model with all tasks in the system */
    //TasksFilterModel    _tasksFilterModel;      /** Filter model for tasks model */
};

}
