// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "GlobalSettingsGroupAction.h"

namespace mv {
    namespace plugin {
        class PluginFactory;
    }
}

namespace mv::gui
{

/**
 * Plugin global settings group action class
 *
 * Global settings group action class for global plugin settings
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
 * @author Thomas Kroes
 */
class PluginGlobalSettingsGroupAction : public GlobalSettingsGroupAction
{
public:

    /**
     * Construct with pointer to \p parent object and \p pluginFactory
     * @param parent Pointer to parent object
     * @param pluginFactory Pointer to plugin factory
     */
    PluginGlobalSettingsGroupAction(QObject* parent, const plugin::PluginFactory* pluginFactory);
};

}
