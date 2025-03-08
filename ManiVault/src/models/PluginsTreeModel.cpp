// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PluginsTreeModel.h"

#include "CoreInterface.h"

using namespace mv;

#ifdef _DEBUG
    #define PLUGINS_TREE_MODEL_VERBOSE
#endif

using namespace mv::util;

namespace mv {

PluginsTreeModel::PluginsTreeModel(PopulationMode populationMode /*= PopulationMode::Automatic*/, QObject* parent /*= nullptr*/) :
    AbstractPluginsModel(populationMode, parent)
{
    setColumnCount(static_cast<int>(AbstractPluginsModel::Column::Count));

    PluginsTreeModel::populateFromPluginManager();
}

plugin::Plugins PluginsTreeModel::getPlugins() const
{
    return {};
}

plugin::Plugin* PluginsTreeModel::getPlugin(const QModelIndex& modelIndex) const
{
    return {};
}

void PluginsTreeModel::setPlugins(const plugin::Plugins& plugins)
{
}

void PluginsTreeModel::populateFromPluginManager()
{
    const auto pluginTypes = plugin::Types{
        plugin::Type::ANALYSIS,
        plugin::Type::DATA,
        plugin::Type::LOADER,
        plugin::Type::WRITER,
        plugin::Type::TRANSFORMATION,
        plugin::Type::VIEW
    };

    for (auto pluginType : pluginTypes) {
        auto pluginTypeRow = Row(nullptr, QString("%1 plugins").arg(getPluginTypeName(pluginType)), "Type", "", getPluginTypeIcon(pluginType));

        appendRow(pluginTypeRow);

        for (auto pluginFactory : plugins().getPluginFactoriesByType(pluginType)) {
            auto pluginFactoryRow = Row(nullptr, pluginFactory->getKind(), "Factory", "", StyledIcon(pluginFactory->icon()));

            pluginFactoryRow.first()->setEnabled(false);
            pluginFactoryRow.first()->setEditable(false);

            pluginTypeRow.first()->appendRow(pluginFactoryRow);

            for (auto plugin : plugins().getPluginsByFactory(pluginFactory))
                pluginFactoryRow.first()->appendRow(Row(plugin, plugin->getGuiName(), "Instance", plugin->getId(), StyledIcon(plugin->icon())));
        }
    }
}

void PluginsTreeModel::addPlugin(plugin::Plugin* plugin)
{
    Q_ASSERT(plugin);

    if (!plugin)
        return;

    const auto matches = match(index(0, static_cast<int>(AbstractPluginsModel::Column::Name)), Qt::EditRole, plugin->getKind(), 1, Qt::MatchExactly | Qt::MatchRecursive);

    if (matches.isEmpty())
        return;

    auto pluginRow = new QStandardItem(plugin->getGuiName());
    auto pluginId = new QStandardItem(plugin->getId());

    pluginId->setEditable(false);

    pluginRow->setData(QVariant::fromValue(plugin));
    pluginRow->setEditable(false);

    itemFromIndex(matches.first())->appendRow({ pluginRow, new QStandardItem("Instance"), pluginId });
}

void PluginsTreeModel::removePlugin(plugin::Plugin* plugin)
{
    Q_ASSERT(plugin);

    if (!plugin)
        return;

    const auto matches = match(index(0, static_cast<int>(AbstractPluginsModel::Column::Id)), Qt::EditRole, plugin->getId(), 1, Qt::MatchExactly | Qt::MatchRecursive);

    if (matches.isEmpty())
        return;

    removeRow(matches.first().row());
}

}
