// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/GroupAction.h"

namespace mv::gui
{

/**
 * @brief Base action for a group of application or plugin-wide settings.
 *
 * Base action class which groups settings for
 * - Common global settings (e.g. tasks & parameters)
 * - Plugin global settings
 * 
 * This class assigns a global-settings prefix to child actions added to the group,
 * keeping their serialized values under the group name. Child actions are referenced
 * by the group and must remain alive while registered.
 * 
 * Settings are stored under ManiVault/GlobalSettings/<global_settings_name>/<action_name>.
 * The advanced flag classifies the whole group for settings-manager filtering; it does
 * not change visibility of individual child actions and is not serialized.
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

    /**
     * @brief Gets the prefix used for this global settings group.
     * @return Prefix in the form ManiVault/GlobalSettings/<group-name>/
     */
    QString getSettingsPrefix() const override;

public: // Advanced group

    /**
     * @brief Sets whether the whole settings group is classified as advanced.
     *
     * The Settings Manager can use this flag to hide the group when advanced settings
     * are excluded. This metadata is not saved with the settings values.
     * @param advanced Whether the group should be classified as advanced
     */
    void setAdvanced(bool advanced);

    /**
     * @brief Gets whether the whole settings group is classified as advanced.
     * @return Whether the group has been marked advanced; false by default
     */
    bool isAdvanced() const;

public: // Actions management

    /**
     * Add an action and assign its settings prefix when it is a child of this group.
     * The group stores a non-owning pointer; the action must remain alive while registered.
     * @param action Child action to add
     * @param widgetFlags Action widget flags (default flags if -1)
     * @param widgetConfigurationFunction When set, overrides the standard widget configuration function in the widget action
     * @param load Reserved parameter; this override currently ignores it
     */
    void addAction(WidgetAction* action, std::int32_t widgetFlags = -1, mv::gui::WidgetConfigurationFunction widgetConfigurationFunction = mv::gui::WidgetConfigurationFunction(), bool load = true) override;

private:
    bool _advanced = false; /**< Non-serialized classification flag for the whole group; false by default. */
};

}
