// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ScriptingManager.h"

using namespace mv::gui;
using namespace mv::util;

#ifdef _DEBUG
    //#define SCRIPTING_MANAGER_VERBOSE
#endif

namespace mv
{

ScriptingManager::ScriptingManager(QObject* parent) :
    AbstractScriptingManager(parent)
{
}

ScriptingManager::~ScriptingManager()
{
    reset();
}

void ScriptingManager::initialize()
{
#ifdef HELP_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    AbstractScriptingManager::initialize();

    if (isInitialized())
        return;

    beginInitialization();
    {
        
    }
    endInitialization();
}

void ScriptingManager::reset()
{
#ifdef HELP_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    beginReset();
    {
    }
    endReset();
}

ScriptTriggerActions ScriptingManager::getScriptTriggerActions(const Datasets& datasets) const
{
    ScriptTriggerActions scriptTriggerActions;

    for (auto pluginFactory : mv::plugins().getPluginFactoriesByTypes())
        scriptTriggerActions << pluginFactory->getScriptTriggerActions(datasets);

    for (auto plugin : mv::plugins().getPluginsByTypes())
        scriptTriggerActions << plugin->getScriptTriggerActions(datasets);

    sortActions(scriptTriggerActions);

    return scriptTriggerActions;
}

}
