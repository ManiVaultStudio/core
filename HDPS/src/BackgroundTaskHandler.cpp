// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "BackgroundTaskHandler.h"

namespace mv {

BackgroundTaskHandler::BackgroundTaskHandler(QObject* parent) :
    AbstractTaskHandler(parent, nullptr),
    _overallBackgroundTask(this, "Background tasks", false)
{
}

mv::BackgroundTask& BackgroundTaskHandler::getOverallBackgroundTask()
{
    return _overallBackgroundTask;
}

}
