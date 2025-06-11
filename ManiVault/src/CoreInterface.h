// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "PluginType.h"
#include "Dataset.h"
#include "Application.h"

#include "AbstractActionsManager.h"
#include "AbstractThemeManager.h"
#include "AbstractPluginManager.h"
#include "AbstractDataManager.h"
#include "AbstractDataHierarchyManager.h"
#include "AbstractEventManager.h"
#include "AbstractTaskManager.h"
#include "AbstractWorkspaceManager.h"
#include "AbstractProjectManager.h"
#include "AbstractSettingsManager.h"
#include "AbstractHelpManager.h"
#include "AbstractErrorManager.h"

#include <QString>
#include <QObject>

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

class CORE_EXPORT CoreInterface : public QObject
{
    Q_OBJECT

public:

    /** Enumeration for distinguishing manager types */
    enum class ManagerType {
        Errors = 0,         /** Manager for errors */
        Actions,            /** Actions manager for storing actions */
        Theme,              /** Manager for controlling the application theme */
        Plugins,            /** Plugin manager responsible for loading plug-ins and adding them to the core */
        Events,             /** Event manager for emitting global events */
        Data,               /** Data manager responsible for storing data sets and data selections */
        DataHierarchy,      /** Data hierarchy manager for providing a hierarchical dataset structure */
        Tasks,              /** Manager for managing global tasks */
        Workspaces,         /** Workspace manager for controlling widgets layout */
        Projects,           /** Manager for loading/saving projects */
        Settings,           /** Manager for managing global settings */
        Help,               /** Manager for getting help */

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

    /**
     * Get whether the core is about to be destroyed or not
     * @return Boolean determining whether the core is about to be destroyed or not
     */
    virtual bool isAboutToBeDestroyed() const = 0;

public: // Managers

    virtual AbstractManager* getManager(const ManagerType& managerType) = 0;

    virtual AbstractErrorManager& getErrorManager() = 0;
    virtual AbstractActionsManager& getActionsManager() = 0;
    virtual AbstractThemeManager& getThemeManager() = 0;
    virtual AbstractPluginManager& getPluginManager() = 0;
    virtual AbstractEventManager& getEventManager() = 0;
    virtual AbstractDataManager& getDataManager() = 0;
    virtual AbstractDataHierarchyManager& getDataHierarchyManager() = 0;
    virtual AbstractTaskManager& getTaskManager() = 0;
    virtual AbstractWorkspaceManager& getWorkspaceManager() = 0;
    virtual AbstractProjectManager& getProjectManager() = 0;
    virtual AbstractSettingsManager& getSettingsManager() = 0;
    virtual AbstractHelpManager& getHelpManager() = 0;

signals:

    /** Invoked when the core is about to be initialized */
    void aboutToBeInitialized();

    /** Invoked when the core is about to be destroyed */
    void aboutToBeDestroyed();

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
CORE_EXPORT inline CoreInterface* core() {
    return Application::core();
}

/**
* Convenience function to obtain access to the error manager in the core
* @return Reference to abstract error manager
*/
CORE_EXPORT inline AbstractErrorManager& errors() {
    return core()->getErrorManager();
}

/**
 * Convenience function to obtain access to the actions manager in the core
 * @return Reference to abstract actions manager
 */
CORE_EXPORT inline AbstractActionsManager& actions() {
    return core()->getActionsManager();
}

/**
* Convenience function to obtain access to the theme manager in the core
* @return Reference to abstract theme manager
*/
CORE_EXPORT inline AbstractThemeManager& theme() {
    return core()->getThemeManager();
}

/**
 * Convenience function to obtain access to the plugin manager in the core
 * @return Reference to abstract plugin manager
 */
CORE_EXPORT inline AbstractPluginManager& plugins() {
    return core()->getPluginManager();
}

/**
 * Convenience function to obtain access to the event manager in the core
 * @return Reference to abstract event manager
 */
CORE_EXPORT inline AbstractEventManager& events() {
    return core()->getEventManager();
}

/**
 * Convenience function to obtain access to the data manager in the core
 * @return Reference to abstract data manager
 */
CORE_EXPORT inline AbstractDataManager& data() {
    return core()->getDataManager();
}

/**
 * Convenience function to obtain access to the data hierarchy manager in the core
 * @return Reference to abstract data hierarchy manager
 */
CORE_EXPORT inline AbstractDataHierarchyManager& dataHierarchy() {
    return core()->getDataHierarchyManager();
}

/**
 * Convenience function to obtain access to the workspace manager in the core
 * @return Reference to abstract workspace manager
 */
CORE_EXPORT inline AbstractWorkspaceManager& workspaces() {
    return core()->getWorkspaceManager();
}

/**
 * Convenience function to obtain access to the task manager in the core
 * @return Reference to abstract task manager
 */
CORE_EXPORT inline AbstractTaskManager& tasks() {
    return core()->getTaskManager();
}

/**
 * Convenience function to obtain access to the project manager in the core
 * @return Reference to abstract project manager
 */
CORE_EXPORT inline AbstractProjectManager& projects() {
    return core()->getProjectManager();
}

/**
 * Convenience function to obtain access to the settings manager in the core
 * @return Reference to abstract settings manager
 */
CORE_EXPORT inline AbstractSettingsManager& settings() {
    return core()->getSettingsManager();
}

/**
 * Convenience function to obtain access to the help manager in the core
 * @return Reference to abstract help manager
 */
CORE_EXPORT inline AbstractHelpManager& help() {
    return core()->getHelpManager();
}

}
