// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ForegroundTaskHandler.h"

#include "actions/ForegroundTasksStatusBarAction.h"

namespace mv {

using namespace gui;

ForegroundTaskHandler::ForegroundTaskHandler(QObject* parent) :
    AbstractTaskHandler(parent, nullptr),
    _statusBarAction(nullptr)
{
}

mv::gui::WidgetAction* ForegroundTaskHandler::getStatusBarAction()
{
    if (!_statusBarAction)
        _statusBarAction = new ForegroundTasksStatusBarAction(this, "Status bar");

    return _statusBarAction;
}

}
