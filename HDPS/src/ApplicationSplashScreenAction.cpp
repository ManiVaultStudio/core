// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ApplicationSplashScreenAction.h"
#include "ApplicationSplashScreenDialog.h"
#include "Application.h"

namespace hdps::gui {

ApplicationSplashScreenAction::ApplicationSplashScreenAction(QObject* parent) :
    AbstractSplashScreenAction(parent)
{
    connect(Application::current()->getTask(Application::TaskType::LoadApplication), &Task::statusChanged, this, [this](const Task::Status& previousStatus, const Task::Status& status) -> void {
        if (previousStatus == Task::Status::Finished && status == Task::Status::Idle)
            AbstractSplashScreenAction::closeSplashScreenDialog();
    });
}

SplashScreenDialog* ApplicationSplashScreenAction::getSplashScreenDialog()
{
    return new ApplicationSplashScreenDialog(this);
}

}
