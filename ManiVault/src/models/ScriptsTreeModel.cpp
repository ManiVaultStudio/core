// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ScriptsTreeModel.h"

using namespace mv;

#ifdef _DEBUG
    #define SCRIPTS_TREE_MODEL_VERBOSE
#endif

namespace mv {

ScriptsTreeModel::PluginFactoriesTreeModel(QObject* parent /*= nullptr*/) :
    AbstractScriptsModel(parent)
{
    /*const auto pluginTypes = plugin::Types{
        plugin::Type::ANALYSIS,
        plugin::Type::DATA,
        plugin::Type::LOADER,
        plugin::Type::WRITER,
        plugin::Type::TRANSFORMATION,
        plugin::Type::VIEW
    };

    for (auto pluginType : pluginTypes) {
        Row pluginTypeRow(getPluginTypeName(pluginType));

        appendRow(pluginTypeRow);

        for (auto pluginFactory : plugins().getPluginFactoriesByType(pluginType))
            pluginTypeRow.first()->appendRow(Row(pluginFactory));

        pluginTypeRow.first()->setEnabled(pluginTypeRow.first()->hasChildren());
    }*/
}

}
