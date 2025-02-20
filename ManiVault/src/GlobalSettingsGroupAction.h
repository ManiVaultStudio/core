// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/GroupAction.h"

namespace mv::gui
{

/**
 * Global settings group action class
 *
 * Base action class which groups settings for
 * - Common global settings (e.g. tasks & parameters)
 * - Plugin global settings
 * 
 * This class ensures that settings are store in the correct place because it overrides the GroupAction::addAction(...)
 * and adjusts the settings prefix of the added actions.
 * 
 * Settings can be found in: ManiVault/GlobalSettings/<global_settings_name>/<action_name>
 * 
 * Procedure(s) for adding a common global setting:
 * a. Add an action to an existing global settings group:
 *     - ApplicationSettingsAction::addAction(...)
 *     - MiscellaneousSettingsAction::addAction(...)
 *     - ParametersSettingsAction::addAction(...)
 *     - TasksSettingsAction::addAction(...)
 *     - TemporaryDirectoriesSettingsAction::addAction(...)
 * b. Create a new settings action derived from GlobalSettingsGroupAction, add it to the SettingsManager
 *    class and add actions to it: MySettingsAction::addAction(...)
 * 
 * Procedure for adding a plugin global setting:
 * a. If a plugin does not have global settings yet:
 *     1. Create a settings action (e.g. MyPluginGlobalSettingsAction) derived from PluginGlobalSettingsGroupAction
 *     2. Add actions to MyPluginGlobalSettingsAction in its constructor (MyPluginGlobalSettingsAction::addAction(...))
 *     3. Create an instance of MyPluginGlobalSettingsAction, and assign it to the plugin factory in the initialize()
 *        member function of the plugin factory: setGlobalSettingsGroupAction(new MyPluginGlobalSettingsAction(this, this));
 * b. If a plugin already has global settings:
 *     - Add actions to the particular settings action (derived from PluginGlobalSettingsGroupAction) 
 *
 * Procedure for retrieving global settings:
 * Common global setting example: mv::settings().getMiscellaneousSettings().getAskConfirmationBeforeRemovingDatasetsAction()
 * Plugin global setting example: mv::settings().getPluginGlobalSettingsGroupAction<GlobalSettingsAction>(_exampleViewGLPlugin)->getDefaultPointSizeAction().getValue()
 * 
 * @author Thomas Kroes
 */
class CORE_EXPORT GlobalSettingsGroupAction : public GroupAction
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param expanded Boolean determining whether the group action is expanded by default
     */
    GlobalSettingsGroupAction(QObject* parent, const QString& title, bool expanded = true);

    /** Get settings prefix for child actions */
    QString getSettingsPrefix() const override;

public: // Actions management

    /**
     * Add \p action to the group (facade for GroupAction that automatically sets the WidgetAction settings prefix)
     * @param action Pointer to action to add
     * @param widgetFlags Action widget flags (default flags if -1)
     * @param widgetConfigurationFunction When set, overrides the standard widget configuration function in the widget action
     * @param loadSettings Load settings
     */
    void addAction(WidgetAction* action, std::int32_t widgetFlags = -1, mv::gui::WidgetConfigurationFunction widgetConfigurationFunction = mv::gui::WidgetConfigurationFunction(), bool loadSettings = true) override final;
};

}
