// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "PluginType.h"
#include "Dataset.h"
#include "Application.h"

#include "AbstractActionsManager.h"
#include "AbstractPluginManager.h"
#include "AbstractDataManager.h"
#include "AbstractDataHierarchyManager.h"
#include "AbstractEventManager.h"
#include "AbstractTaskManager.h"
#include "AbstractWorkspaceManager.h"
#include "AbstractProjectManager.h"
#include "AbstractSettingsManager.h"

#include <QString>
#include <QObject>

#include <vector>
#include <functional>

namespace mv
{
    class DatasetImpl;
    class DataType;
    class EventListener;
    class DataHierarchyItem;

    namespace plugin
    {
        class Plugin;
        class RawData;
        class LoaderPlugin;
        class WriterPlugin;
        class AnalysisPlugin;
        class ViewPlugin;
        class TransformationPlugin;
    }

    namespace gui
    {
        class PluginTriggerAction;

        using PluginTriggerActions = QVector<QPointer<PluginTriggerAction>>;
    }

class CoreInterface : public QObject
{
    Q_OBJECT

public:

    /** Enumeration for distinguishing manager types */
    enum class ManagerType {
        Actions = 0,        /** Actions manager for storing actions */
        Plugins,            /** Plugin manager responsible for loading plug-ins and adding them to the core */
        Events,             /** Event manager for emitting global events */
        Data,               /** Data manager responsible for storing data sets and data selections */
        DataHierarchy,      /** Data hierarchy manager for providing a hierarchical dataset structure */
        Tasks,              /** Manager for managing global tasks */
        Workspaces,         /** Workspace manager for controlling widgets layout */
        Projects,           /** Manager for loading/saving projects */
        Settings,           /** Manager for managing global settings */

        Count
    };

public:

    /** Default constructor */
    CoreInterface() = default;
    
    /** Creates the core managers */
    virtual void createManagers() = 0;

    /** Flag the core managers as created and notify listeners */
    virtual void setManagersCreated() = 0;

    /** Resets the entire core implementation */
    virtual void reset() = 0;

public: // Initialization

    /** Initializes the core */
    virtual void initialize() = 0;

    /** Notify listeners that the core is about to be initialized */
    virtual void setAboutToBeInitialized() = 0;

    /** Flag the core as initialized and notify listeners */
    virtual void setInitialized() = 0;

    /**
     * Get whether the core is initialized or not
     * @return Boolean determining whether the core is initialized or not
     */
    virtual bool isInitialized() const = 0;

public: // Managers

    virtual AbstractManager* getManager(const ManagerType& managerType) = 0;

    virtual AbstractActionsManager& getActionsManager() = 0;
    virtual AbstractPluginManager& getPluginManager() = 0;
    virtual AbstractEventManager& getEventManager() = 0;
    virtual AbstractDataManager& getDataManager() = 0;
    virtual AbstractDataHierarchyManager& getDataHierarchyManager() = 0;
    virtual AbstractTaskManager& getTaskManager() = 0;
    virtual AbstractWorkspaceManager& getWorkspaceManager() = 0;
    virtual AbstractProjectManager& getProjectManager() = 0;
    virtual AbstractSettingsManager& getSettingsManager() = 0;

signals:

    /** Invoked when the core is about to be initialized */
    void aboutToBeInitialized();

    /** Invoked when the core has been initialized */
    void initialized();

    /** Invoked when the core managers have been created */
    void managersCreated();

    friend class plugin::RawData;
    friend class DatasetImpl;
    friend class EventListener;
};

/**
 * Convenience function to obtain access to the core
 * @return Pointer to the current instance of the core
 */
static CoreInterface* core() {
    return Application::core();
}

/**
 * Convenience function to obtain access to the actions manager in the core
 * @return Reference to abstract actions manager
 */
static AbstractActionsManager& actions() {
    return core()->getActionsManager();
}

/**
 * Convenience function to obtain access to the plugin manager in the core
 * @return Reference to abstract plugin manager
 */
static AbstractPluginManager& plugins() {
    return core()->getPluginManager();
}

/**
 * Convenience function to obtain access to the event manager in the core
 * @return Reference to abstract event manager
 */
static AbstractEventManager& events() {
    return core()->getEventManager();
}

/**
 * Convenience function to obtain access to the data manager in the core
 * @return Reference to abstract data manager
 */
static AbstractDataManager& data() {
    return core()->getDataManager();
}

/**
 * Convenience function to obtain access to the data hierarchy manager in the core
 * @return Reference to abstract data hierarchy manager
 */
static AbstractDataHierarchyManager& dataHierarchy() {
    return core()->getDataHierarchyManager();
}

/**
 * Convenience function to obtain access to the workspace manager in the core
 * @return Reference to abstract workspace manager
 */
static AbstractWorkspaceManager& workspaces() {
    return core()->getWorkspaceManager();
}

/**
 * Convenience function to obtain access to the task manager in the core
 * @return Reference to abstract task manager
 */
static AbstractTaskManager& tasks() {
    return core()->getTaskManager();
}

/**
 * Convenience function to obtain access to the project manager in the core
 * @return Reference to abstract project manager
 */
static AbstractProjectManager& projects() {
    return core()->getProjectManager();
}

/**
 * Convenience function to obtain access to the settings manager in the core
 * @return Reference to abstract settings manager
 */
static AbstractSettingsManager& settings() {
    return core()->getSettingsManager();
}

}