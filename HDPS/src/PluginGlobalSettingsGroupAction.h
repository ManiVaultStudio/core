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
