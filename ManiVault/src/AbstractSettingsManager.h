// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractManager.h"

#include "actions/TriggerAction.h"

#include "ParametersSettingsAction.h"
#include "MiscellaneousSettingsAction.h"
#include "TasksSettingsAction.h"
#include "AppearanceSettingsAction.h"
#include "TemporaryDirectoriesSettingsAction.h"
#include "PluginGlobalSettingsGroupAction.h"

namespace mv {
    namespace plugin {
        class Plugin;
    }
}

namespace mv
{

/**
 * Abstract settings manager
 *
 * Base abstract settings manager class for managing global settings
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT AbstractSettingsManager : public AbstractManager
{
    Q_OBJECT

public:

    /**
     * Construct manager with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    AbstractSettingsManager(QObject* parent) :
        AbstractManager(parent, "Settings")
    {
    }

    /** Opens the settings editor dialog */
    virtual void edit() = 0;

public: // Action getters

    virtual gui::TriggerAction& getEditSettingsAction() = 0;

public: // Global settings actions

    virtual gui::ParametersSettingsAction& getParametersSettings() = 0;
    virtual gui::MiscellaneousSettingsAction& getMiscellaneousSettings() = 0;
    virtual gui::TasksSettingsAction& getTasksSettingsAction() = 0;
    virtual gui::AppearanceSettingsAction& getAppearanceSettingsAction() = 0;
    virtual gui::TemporaryDirectoriesSettingsAction& getTemporaryDirectoriesSettingsAction() = 0;

    /**
     * Get plugin global settings for plugin \p kind
     * @param kind Plugin kind
     * @return Pointer to plugin global settings (if available, otherwise returns a nullptr)
     */
    virtual gui::PluginGlobalSettingsGroupAction* getPluginGlobalSettingsGroupAction(const QString& kind) = 0;

    /**
     * Get plugin global settings \p ActionType for plugin \p kind
     * @param kind Plugin kind
     * @return Pointer to plugin global settings of \p ActionType (if available, otherwise returns a nullptr)
     */
    template<typename ActionType>
    ActionType* getPluginGlobalSettingsGroupAction(const QString& kind) {
        return dynamic_cast<ActionType*>(getPluginGlobalSettingsGroupAction(kind));
    }

    /**
    * Get plugin global settings for \p plugin
    * @param plugin Pointer to plugin
    * @return Pointer to plugin global settings (if available, otherwise returns a nullptr)
    */
    virtual gui::PluginGlobalSettingsGroupAction* getPluginGlobalSettingsGroupAction(const plugin::Plugin* plugin) = 0;

    /**
    * Get plugin global settings of \p ActionType for \p plugin
    * @param plugin Pointer to plugin
    * @return Pointer to plugin global settings of \p ActionType (if available, otherwise returns a nullptr)
    */
    template<typename ActionType>
    ActionType* getPluginGlobalSettingsGroupAction(const plugin::Plugin* plugin) {
        return dynamic_cast<ActionType*>(getPluginGlobalSettingsGroupAction(plugin));
    }
};

}
