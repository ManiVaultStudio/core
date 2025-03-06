// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PluginsListModel.h"

#include "CoreInterface.h"

using namespace mv;

#ifdef _DEBUG
    #define PLUGINS_LIST_MODEL_VERBOSE
#endif

using namespace mv::util;

namespace mv {

PluginsListModel::PluginsListModel(PopulationMode populationMode /*= PopulationMode::Automatic*/, QObject* parent /*= nullptr*/) :
    AbstractPluginsModel(populationMode, parent)
{
    setColumnCount(static_cast<int>(Column::Count));

    PluginsListModel::populateFromPluginManager();
}

plugin::Plugins PluginsListModel::getPlugins() const
{
    plugin::Plugins plugins;

    for (int rowIndex = 0; rowIndex < rowCount(); rowIndex++)
        plugins.push_back(dynamic_cast<Item*>(itemFromIndex(index(rowIndex, 0)))->getPlugin());

    return plugins;
}

plugin::Plugin* PluginsListModel::getPlugin(const QModelIndex& modelIndex) const
{
    auto item = dynamic_cast<Item*>(itemFromIndex(modelIndex.siblingAtColumn(static_cast<int>(AbstractPluginsModel::Column::Name))));

    if (!item)
        return {};

    return item->getPlugin();
}

void PluginsListModel::setPlugins(const plugin::Plugins& plugins)
{
    setRowCount(0);

    for (const auto plugin : plugins)
        addPlugin(plugin);
}

void PluginsListModel::populateFromPluginManager()
{
    setPlugins(mv::plugins().getPluginsByTypes());
}

void PluginsListModel::addPlugin(plugin::Plugin* plugin)
{
    Q_ASSERT(plugin);

    if (!plugin)
        return;

    appendRow(Row(plugin, plugin->getGuiName(), "Instance", plugin->getId(), StyledIcon(plugin->icon())));
}

void PluginsListModel::removePlugin(plugin::Plugin* plugin)
{
    Q_ASSERT(plugin);

    if (!plugin)
        return;

    const auto matches = match(index(0, static_cast<int>(Column::Id)), Qt::DisplayRole, plugin->getId(), 1, Qt::MatchExactly | Qt::MatchRecursive);

    if (matches.isEmpty())
        return;

    removeRow(matches.first().row());
}

QModelIndex PluginsListModel::getIndexFromPlugin(const plugin::Plugin* plugin) const
{
    try {
        Q_ASSERT(plugin);

        if (!plugin)
            throw std::runtime_error("Plugin is nullptr");

        return getItemFromPlugin(plugin)->index();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to get index from plugin in plugins list model", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to get index from plugin in plugins list model");
    }

    return {};
}

QModelIndex PluginsListModel::getIndexFromPlugin(const QString& pluginId) const
{
    try {
        const auto matches = match(index(0, static_cast<int>(AbstractPluginsModel::Column::Id)), Qt::EditRole, pluginId, 1, Qt::MatchExactly | Qt::MatchRecursive);

        if (matches.isEmpty())
            return {};

        return matches.first();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to get index from plugin ID in plugins model", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to get index from plugin ID in plugins model");
    }

    return {};
}

QStandardItem* PluginsListModel::getItemFromPlugin(const plugin::Plugin* plugin) const
{
    if (!plugin)
        throw std::runtime_error("Supplied plugin is not valid");

    const auto matches = match(index(0, static_cast<int>(Column::Id)), Qt::EditRole, plugin->getId(), 1, Qt::MatchExactly | Qt::MatchRecursive);

    if (matches.isEmpty())
        throw std::runtime_error(QString("%1 (%2) not found").arg(plugin->getGuiName(), plugin->getId()).toStdString());

    return itemFromIndex(matches.first().siblingAtColumn(static_cast<int>(Column::Name)));
}

}
