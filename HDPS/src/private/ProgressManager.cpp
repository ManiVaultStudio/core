// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProgressManager.h"

#ifdef _DEBUG
    #define PROGRESS_MANAGER_VERBOSE
#endif

namespace hdps
{

ProgressManager::ProgressManager(QObject* parent /*= nullptr*/) :
    AbstractProgressManager()
{
}

ProgressManager::~ProgressManager()
{
    reset();
}

void ProgressManager::initialize()
{
#ifdef PROGRESS_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    AbstractProgressManager::initialize();
}

void ProgressManager::reset()
{
#ifdef PROGRESS_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    beginReset();
    {
    }
    endReset();
}

}
