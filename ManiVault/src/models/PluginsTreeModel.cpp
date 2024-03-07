// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PluginsTreeModel.h"

#include "Application.h"
#include "CoreInterface.h"
#include "AbstractPluginManager.h"

using namespace mv;

#ifdef _DEBUG
    #define PLUGINS_TREE_MODEL_VERBOSE
#endif

namespace mv {

PluginsTreeModel::PluginsTreeModel(QObject* parent /*= nullptr*/) :
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
        auto pluginTypeRow = Row(QString("%1 plugins").arg(getPluginTypeName(pluginType)), "Type", "", getPluginTypeIcon(pluginType));

        appendRow(pluginTypeRow);

        for (auto pluginFactory : plugins().getPluginFactoriesByType(pluginType)) {
            auto pluginFactoryRow = Row(pluginFactory->getKind(), "Factory", "", pluginFactory->getIcon());

            pluginFactoryRow.first()->setEnabled(false);
            pluginFactoryRow.first()->setEditable(false);

            pluginTypeRow.first()->appendRow(pluginFactoryRow);

            for (auto plugin : plugins().getPluginsByFactory(pluginFactory)) {
                auto pluginRow = new QStandardItem(plugin->getGuiName());
                auto pluginId = new QStandardItem(plugin->getId());

                pluginId->setEditable(false);

                pluginRow->setData(QVariant::fromValue(plugin));
                pluginRow->setEditable(false);

                pluginFactoryRow.first()->appendRow({ pluginRow, new QStandardItem("Instance"), pluginId });

                pluginTypeRow.first()->setEnabled(true);
                pluginFactoryRow.first()->setEnabled(true);
            }
        }
    }
}

}
