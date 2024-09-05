// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PluginsListModel.h"

#include "CoreInterface.h"

using namespace mv;

#ifdef _DEBUG
    #define PLUGINS_LIST_MODEL_VERBOSE
#endif

namespace mv {

PluginsListModel::PluginsListModel(QObject* parent /*= nullptr*/) :
    AbstractPluginsModel(parent)
{
    setColumnCount(static_cast<int>(AbstractPluginsModel::Column::Count));

    const auto pluginTypes = plugin::Types{
        plugin::Type::ANALYSIS,
        plugin::Type::DATA,
        plugin::Type::LOADER,
        plugin::Type::WRITER,
        plugin::Type::TRANSFORMATION,
        plugin::Type::VIEW
    };

    for (auto pluginType : pluginTypes) {
        for (auto pluginFactory : plugins().getPluginFactoriesByType(pluginType)) {
            for (auto plugin : plugins().getPluginsByFactory(pluginFactory)) {
                auto pluginRow = new QStandardItem(plugin->getGuiName());
                auto pluginId = new QStandardItem(plugin->getId());

                pluginId->setEditable(false);

                pluginRow->setData(QVariant::fromValue(plugin));
                pluginRow->setEditable(false);

                appendRow({ pluginRow, new QStandardItem("Instance"), pluginId });
            }
        }
    }
}

}
