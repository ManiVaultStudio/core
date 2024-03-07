// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractSettingsManager.h"

namespace mv
{

class SettingsManager final : public AbstractSettingsManager
{
    Q_OBJECT

public:

    /** Default constructor */
    SettingsManager();

    /** Perform manager startup initialization */
    void initialize() override;

    /** Resets the contents of the actions manager */
    void reset() override;

    /** Opens the settings editor dialog */
    void edit() override;

public: // Action getters

    gui::TriggerAction& getEditSettingsAction() override { return _editSettingsAction; }

public: // Global settings actions

    gui::ParametersSettingsAction& getParametersSettings() override { return _parametersSettingsAction; };
    gui::MiscellaneousSettingsAction& getMiscellaneousSettings() override { return _miscellaneousSettingsAction; };
    gui::TasksSettingsAction& getTasksSettingsAction() override { return _tasksSettingsAction; };
    gui::ApplicationSettingsAction& getApplicationSettings() override { return _applicationSettingsAction; };
    gui::TemporaryDirectoriesSettingsAction& getTemporaryDirectoriesSettingsAction() override { return _temporaryDirectoriesSettingsAction; };

    /**
     * Get plugin global settings for plugin \p kind
     * @param kind Plugin kind
     * @return Pointer to plugin global settings (if available, otherwise returns a nullptr)
     */
    gui::PluginGlobalSettingsGroupAction* getPluginGlobalSettingsGroupAction(const QString& kind) override;

    /**
    * Get plugin global settings for \p plugin
    * @param plugin Pointer to plugin
    * @return Pointer to plugin global settings (if available, otherwise returns a nullptr)
    */
    gui::PluginGlobalSettingsGroupAction* getPluginGlobalSettingsGroupAction(const plugin::Plugin* plugin) override;

private:
    gui::TriggerAction                          _editSettingsAction;                    /** Action for triggering the settings dialog */
    gui::ParametersSettingsAction               _parametersSettingsAction;              /** Parameters global settings */
    gui::MiscellaneousSettingsAction            _miscellaneousSettingsAction;           /** Miscellaneous global settings */
    gui::TasksSettingsAction                    _tasksSettingsAction;                   /** Tasks global settings */
    gui::ApplicationSettingsAction              _applicationSettingsAction;             /** Application global settings */
    gui::TemporaryDirectoriesSettingsAction     _temporaryDirectoriesSettingsAction;    /** Temporary files global settings */
};

}
