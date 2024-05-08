// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "HelpManager.h"

#include <Application.h>

using namespace mv::gui;
using namespace mv::util;

#ifdef _DEBUG
    //#define HELP_MANAGER_VERBOSE
#endif

namespace mv
{

HelpManager::HelpManager() :
    AbstractHelpManager(),
    _showLearningCenterAction(this, "Learning center")
{
    _showLearningCenterAction.setIconByName("chalkboard-teacher");
    _showLearningCenterAction.setToolTip("Go to the learning center");
}

void HelpManager::initialize()
{
#ifdef HELP_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    AbstractHelpManager::initialize();

    if (isInitialized())
        return;

    beginInitialization();
    endInitialization();
}

void HelpManager::reset()
{
}

}
