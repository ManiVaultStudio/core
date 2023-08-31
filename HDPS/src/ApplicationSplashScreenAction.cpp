// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ApplicationSplashScreenAction.h"
#include "ApplicationSplashScreenDialog.h"
#include "Application.h"

namespace hdps::gui {

ApplicationSplashScreenAction::ApplicationSplashScreenAction(QObject* parent) :
    AbstractSplashScreenAction(parent)//,
    //_tasksModel(this),
    //_tasksFilterModel(this)
{
    getTaskAction().setTask(&Application::current()->getStartupTask());

    //_tasksFilterModel.getTaskStatusFilterAction().setSelectedOptions({ "Running", "Running Indeterminate", "Finished" });
    //_tasksFilterModel.getTaskTypeFilterAction().selectOption("ModalTask");

    connect(&Application::current()->getStartupTask(), &Task::finished, this, [this]() -> void {
        QTimer::singleShot(1500, [this]() -> void {
            AbstractSplashScreenAction::closeSplashScreenDialog();
        });
    });
}

SplashScreenDialog* ApplicationSplashScreenAction::getSplashScreenDialog()
{
    return new ApplicationSplashScreenDialog(this);
}

//TasksModel& ApplicationSplashScreenAction::getTasksModel()
//{
//    return _tasksModel;
//}
//
//TasksFilterModel& ApplicationSplashScreenAction::getTasksFilterModel()
//{
//    return _tasksFilterModel;
//}

}