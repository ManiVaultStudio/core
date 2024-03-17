// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PluginFactoriesListModel.h"

#include "AbstractPluginManager.h"
#include "CoreInterface.h"

using namespace mv;

#ifdef _DEBUG
    #define PLUGIN_FACTORIES_LIST_MODEL_VERBOSE
#endif

namespace mv {

PluginFactoriesListModel::PluginFactoriesListModel(QObject* parent /*= nullptr*/) :
    AbstractPluginFactoriesModel(parent)
{
    const auto pluginTypes = plugin::Types{
        plugin::Type::ANALYSIS,
        plugin::Type::DATA,
        plugin::Type::LOADER,
        plugin::Type::WRITER,
        plugin::Type::TRANSFORMATION,
        plugin::Type::VIEW
    };

    for (auto pluginType : pluginTypes)
        for (auto pluginFactory : plugins().getPluginFactoriesByType(pluginType))
            appendRow(Row(pluginFactory));
}

}
