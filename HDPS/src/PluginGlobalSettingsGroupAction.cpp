// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PluginGlobalSettingsGroupAction.h"

#include "Plugin.h"

namespace mv
{

using namespace gui;

PluginGlobalSettingsGroupAction::PluginGlobalSettingsGroupAction(QObject* parent, const plugin::PluginFactory* pluginFactory) :
    GlobalSettingsGroupAction(parent, pluginFactory->getKind()),
    _pluginFactory(pluginFactory)
{
}

//QString PluginGlobalSettingsGroupAction::getSettingsPrefix() const
//{
//    Q_ASSERT(_pluginFactory != nullptr);
//
//    if (!_pluginFactory)
//        return {};
//
//    return GlobalSettingsGroupAction::getSettingsPrefix() + _pluginFactory->getGlobalSettingsPrefix();
//}

}
