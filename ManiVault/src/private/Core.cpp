// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Core.h"

#include "ErrorManager.h"
#include "ActionsManager.h"
#include "ThemeManager.h"
#include "PluginManager.h"
#include "EventManager.h"
#include "DataManager.h"
#include "DataHierarchyManager.h"
#include "TaskManager.h"
#include "WorkspaceManager.h"
#include "ProjectManager.h"
#include "SettingsManager.h"
#include "HelpManager.h"
#include "ScriptingManager.h"

#include "Application.h"

//#define CORE_VERBOSE

using namespace mv;
using namespace mv::util;
using namespace mv::plugin;
using namespace mv::gui;

namespace mv {

Core::Core() :
    CoreInterface(),
    _initialized(false),
    _aboutToBeDestroyed(false)
{
    connect(Application::current(), &QCoreApplication::aboutToQuit, this, [this]() -> void {
        emit aboutToBeDestroyed();

    	_aboutToBeDestroyed = true;

        for (auto& manager : _managers)
            manager->setCoreIsDestroyed();

    	reset();

        _managers.clear();
    });
}

void Core::createManagers()
{
    _managers.resize(static_cast<int>(ManagerType::Count));

    _managers[static_cast<int>(ManagerType::Errors)]        = std::make_unique<ErrorManager>(this);
    _managers[static_cast<int>(ManagerType::Actions)]       = std::make_unique<ActionsManager>(this);
    _managers[static_cast<int>(ManagerType::Theme)]         = std::make_unique<ThemeManager>(this);
	_managers[static_cast<int>(ManagerType::Plugins)]       = std::make_unique<PluginManager>(this);
    _managers[static_cast<int>(ManagerType::Events)]        = std::make_unique<EventManager>(this);
    _managers[static_cast<int>(ManagerType::Data)]          = std::make_unique<DataManager>(this);
    _managers[static_cast<int>(ManagerType::DataHierarchy)] = std::make_unique<DataHierarchyManager>(this);
    _managers[static_cast<int>(ManagerType::Tasks)]         = std::make_unique<TaskManager>(this);
    _managers[static_cast<int>(ManagerType::Workspaces)]    = std::make_unique<WorkspaceManager>(this);
    _managers[static_cast<int>(ManagerType::Projects)]      = std::make_unique<ProjectManager>(this);
    _managers[static_cast<int>(ManagerType::Settings)]      = std::make_unique<SettingsManager>(this);
    _managers[static_cast<int>(ManagerType::Help)]          = std::make_unique<HelpManager>(this);
    _managers[static_cast<int>(ManagerType::Scripting)]     = std::make_unique<ScriptingManager>(this);

    setManagersCreated();
}

void Core::setManagersCreated()
{
    qDebug() << "ManiVault core managers created";

    emit managersCreated();
}

void Core::reset()
{
    for (auto& manager : _managers)
        manager->reset();
}

void Core::initialize()
{
    if (isInitialized())
        return;

    setAboutToBeInitialized();
    {
        auto& loadCoreManagersTask = Application::current()->getStartupTask().getLoadCoreManagersTask();

        QStringList subtasks;

        for (auto& manager : _managers)
            subtasks << manager->getSerializationName();

        loadCoreManagersTask.setSubtasks(subtasks);
        loadCoreManagersTask.setRunning();

        for (auto& manager : _managers) {
            loadCoreManagersTask.setSubtaskStarted(manager->getSerializationName(), "Initializing " + manager->getSerializationName().toLower() + " manager");
            {
                manager->initialize();
            }
            loadCoreManagersTask.setSubtaskFinished(manager->getSerializationName(), "Initializing " + manager->getSerializationName().toLower() + " manager");
        }
        
        loadCoreManagersTask.setFinished();
    }
    setInitialized();
}

void Core::setAboutToBeInitialized()
{
    qDebug() << "ManiVault core about to be initialized";

    emit aboutToBeInitialized();
}

void Core::setInitialized()
{
    _initialized = true;

    qDebug() << "ManiVault core initialized";

    emit initialized();
}

bool Core::isInitialized() const
{
    return _initialized;
}

bool Core::isAboutToBeDestroyed() const
{
    return _aboutToBeDestroyed;
}

AbstractManager* Core::getManager(const ManagerType& managerType)
{
    return _managers[static_cast<int>(managerType)].get();
}

AbstractErrorManager& Core::getErrorManager()
{
    return *dynamic_cast<AbstractErrorManager*>(getManager(ManagerType::Errors));
}

AbstractActionsManager& Core::getActionsManager()
{
    return *dynamic_cast<AbstractActionsManager*>(getManager(ManagerType::Actions));
}

AbstractThemeManager& Core::getThemeManager()
{
    return *dynamic_cast<AbstractThemeManager*>(getManager(ManagerType::Theme));
}

AbstractPluginManager& Core::getPluginManager()
{
    return *dynamic_cast<AbstractPluginManager*>(getManager(ManagerType::Plugins));
}

AbstractEventManager& Core::getEventManager()
{
    return *dynamic_cast<AbstractEventManager*>(getManager(ManagerType::Events));
}

AbstractDataManager& Core::getDataManager()
{
    return *dynamic_cast<AbstractDataManager*>(getManager(ManagerType::Data));
}

AbstractDataHierarchyManager& Core::getDataHierarchyManager()
{
    return *dynamic_cast<AbstractDataHierarchyManager*>(getManager(ManagerType::DataHierarchy));
}

AbstractWorkspaceManager& Core::getWorkspaceManager()
{
    return *dynamic_cast<AbstractWorkspaceManager*>(getManager(ManagerType::Workspaces));
}

AbstractTaskManager& Core::getTaskManager()
{
    return *dynamic_cast<AbstractTaskManager*>(getManager(ManagerType::Tasks));
}

AbstractProjectManager& Core::getProjectManager()
{
    return *dynamic_cast<AbstractProjectManager*>(getManager(ManagerType::Projects));
}

AbstractSettingsManager& Core::getSettingsManager()
{
    return *dynamic_cast<AbstractSettingsManager*>(getManager(ManagerType::Settings));
}

AbstractHelpManager& Core::getHelpManager()
{
    return *dynamic_cast<AbstractHelpManager*>(getManager(ManagerType::Help));
}

AbstractScriptingManager& Core::getScriptingManager()
{
    return *dynamic_cast<AbstractScriptingManager*>(getManager(ManagerType::Scripting));
}

}
