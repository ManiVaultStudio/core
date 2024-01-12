// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Core.h"

#include "ModalTask.h"
#include "ForegroundTask.h"

#include "ActionsManager.h"
#include "PluginManager.h"
#include "EventManager.h"
#include "DataManager.h"
#include "DataHierarchyManager.h"
#include "TaskManager.h"
#include "WorkspaceManager.h"
#include "ProjectManager.h"
#include "SettingsManager.h"

//#define CORE_VERBOSE

using namespace mv;
using namespace mv::util;
using namespace mv::plugin;
using namespace mv::gui;

namespace mv {

Core::Core() :
    CoreInterface(),
    _initialized(false)
{
}

Core::~Core()
{
    getPluginManager().reset();
}

void Core::createManagers()
{
    _managers.resize(static_cast<int>(ManagerType::Count));

    _managers[static_cast<int>(ManagerType::Actions)]       = new ActionsManager();
    _managers[static_cast<int>(ManagerType::Plugins)]       = new PluginManager();
    _managers[static_cast<int>(ManagerType::Events)]        = new EventManager();
    _managers[static_cast<int>(ManagerType::Data)]          = new DataManager();
    _managers[static_cast<int>(ManagerType::DataHierarchy)] = new DataHierarchyManager();
    _managers[static_cast<int>(ManagerType::Tasks)]         = new TaskManager();
    _managers[static_cast<int>(ManagerType::Workspaces)]    = new WorkspaceManager();
    _managers[static_cast<int>(ManagerType::Projects)]      = new ProjectManager();
    _managers[static_cast<int>(ManagerType::Settings)]      = new SettingsManager();

    CoreInterface::setManagersCreated();
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

    CoreInterface::setAboutToBeInitialized();
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

            QCoreApplication::processEvents();
        }
        
        loadCoreManagersTask.setFinished();
    }
    CoreInterface::setInitialized();
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

AbstractManager* Core::getManager(const ManagerType& managerType)
{
    return _managers[static_cast<int>(managerType)];
}

AbstractActionsManager& Core::getActionsManager()
{
    return *static_cast<AbstractActionsManager*>(getManager(ManagerType::Actions));
}

AbstractPluginManager& Core::getPluginManager()
{
    return *static_cast<AbstractPluginManager*>(getManager(ManagerType::Plugins));
}

AbstractEventManager& Core::getEventManager()
{
    return *static_cast<AbstractEventManager*>(getManager(ManagerType::Events));
}

AbstractDataManager& Core::getDataManager()
{
    return *static_cast<AbstractDataManager*>(getManager(ManagerType::Data));
}

AbstractDataHierarchyManager& Core::getDataHierarchyManager()
{
    return *static_cast<AbstractDataHierarchyManager*>(getManager(ManagerType::DataHierarchy));
}

AbstractWorkspaceManager& Core::getWorkspaceManager()
{
    return *static_cast<AbstractWorkspaceManager*>(getManager(ManagerType::Workspaces));
}

AbstractTaskManager& Core::getTaskManager()
{
    return *static_cast<AbstractTaskManager*>(getManager(ManagerType::Tasks));
}

AbstractProjectManager& Core::getProjectManager()
{
    return *static_cast<AbstractProjectManager*>(getManager(ManagerType::Projects));
}

AbstractSettingsManager& Core::getSettingsManager()
{
    return *static_cast<AbstractSettingsManager*>(getManager(ManagerType::Settings));
}

}
