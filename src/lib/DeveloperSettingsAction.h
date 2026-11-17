// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "GlobalSettingsGroupAction.h"

#include "actions/ToggleAction.h"

namespace mv::gui
{

/**
 * @brief Global settings for developer add-ons.
 *
 * The enable flag is persisted with the global settings. PluginManager uses it at
 * startup to decide whether plugin factories marked as developer add-ons are added
 * to the available plugin set. Changing the flag while the application is running
 * does not load or unload those factories until the next launch.
 * When no value has been saved for this setting, the previous Developer menu
 * visibility setting is used to initialize it for backward compatibility.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT DeveloperSettingsAction final : public GlobalSettingsGroupAction
{
public:

    /**
     * @brief Constructs the developer settings group.
     * @param parent QObject parent that owns this action when non-null
     */
    DeveloperSettingsAction(QObject* parent);

    /**
     * @brief Gets the toggle for enabling developer add-ons.
     * @return Reference to the persisted developer add-ons toggle
     */
    ToggleAction& getEnableDeveloperAddonsAction();

    /**
     * @brief Gets whether developer add-ons are enabled for the next application launch.
     * @return Whether developer add-ons should be added during plugin startup
     */
    bool isDeveloperAddonsEnabled() const;

private:
    ToggleAction _enableDeveloperAddonsAction; /**< Persisted option controlling startup loading of developer add-ons. */
};

}
