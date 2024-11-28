// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <CoreInterface.h>

namespace mv {

class AbstractActionsManager;
class AbstractPluginManager;
class AbstractEventManager;
class AbstractDataManager;
class AbstractDataHierarchyManager;
class AbstractTaskManager;
class AbstractWorkspaceManager;
class AbstractProjectManager;
class AbstractSettingsManager;
class AbstractHelpManager;

class Core final : public CoreInterface
{
public:

    /** Default constructor */
    Core();

    /** Reset plugin managers upon destruction */
    ~Core() override;

public:

    /** Creates the core managers */
    void createManagers() override;

    /** Flag the core managers as created and notify listeners */
    void setManagersCreated() override;

    /** Resets the entire core implementation */
    void reset() override;

public: // Initialization

    /** Initializes the core */
    void initialize() override;

    /** Notify listeners that the core is about to be initialized */
    void setAboutToBeInitialized() override;

    /** Flag the core as initialized and notify listeners */
    void setInitialized() override;

    /**
     * Get whether the core is initialized or not
     * @return Boolean determining whether the core is initialized or not
     */
    bool isInitialized() const override;

    /**
     * Get whether the core is about to be destroyed or not
     * @return Boolean determining whether the core is about to be destroyed or not
     */
    bool isAboutToBeDestroyed() const override;

public: // Managers

    AbstractManager* getManager(const ManagerType& managerType) override;

    AbstractActionsManager& getActionsManager() override;
    AbstractPluginManager& getPluginManager() override;
    AbstractEventManager& getEventManager() override;
    AbstractDataManager& getDataManager() override;
    AbstractDataHierarchyManager& getDataHierarchyManager() override;
    AbstractTaskManager& getTaskManager() override;
    AbstractWorkspaceManager& getWorkspaceManager() override;
    AbstractProjectManager& getProjectManager() override;
    AbstractSettingsManager& getSettingsManager() override;
    AbstractHelpManager& getHelpManager() override;

private:
    std::vector<std::unique_ptr<AbstractManager>>   _managers;              /** All managers in the core */
    bool                                            _initialized;           /** Boolean determining whether the core is initialized or not */
    bool                                            _aboutToBeDestroyed;    /** Boolean determining whether the core is about to be destroyed or not */

    friend class DataHierarchyManager;
};

}
