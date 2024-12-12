// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractPluginsModel.h"

namespace mv {

/**
 * Plugins list model class
 *
 * List model for loaded plugins
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT PluginsListModel : public AbstractPluginsModel
{
public:

    /**
     * Construct with \p populationMode and pointer to \p parent object
     * @param populationMode Population mode
     * @param parent Pointer to parent object
     */
    PluginsListModel(PopulationMode populationMode = PopulationMode::Automatic, QObject* parent = nullptr);

    /**
     * Get plugins
     * @return Vector of plugins
     */
    plugin::Plugins getPlugins() const override;

    /**
     * Get plugin for \p modelIndex
     * @param modelIndex Index to retrieve
     * @return Pointer to plugin (maybe nullptr)
     */
    plugin::Plugin* getPlugin(const QModelIndex& modelIndex) const override;

    /**
     * Set the \p plugins from which can be picked (mode is set to StorageMode::Manual)
     * @param plugins Vector of plugins
     */
    void setPlugins(const plugin::Plugins& plugins) override;

    /** Synchronize the plugins with the core */
    void populateFromPluginManager() override;

    /**
     * Add \p plugin to the model
     * @param plugin Pointer to plugin
     */
    void addPlugin(plugin::Plugin* plugin) override;

    /**
     * Remove \p plugin from the model
     * @param plugin Pointer to plugin
     */
    void removePlugin(plugin::Plugin* plugin) override;

    /**
     * Get index from pointer to \p plugin
     * @param plugin Const pointer to plugin
     * @return Model index (invalid if not found)
     */
    QModelIndex getIndexFromPlugin(const plugin::Plugin* plugin) const;

    /**
     * Get index from \p pluginId
     * @param pluginId Plugin globally unique identifier to retrieve the row index for
     * @return Model index (invalid if not found)
     */
    QModelIndex getIndexFromPlugin(const QString& pluginId) const;

public: // Item access

    /**
     * Get item from pointer to \p plugin
     * @param plugin Const pointer to plugin
     * @return Pointer to found item, nullptr otherwise
     */
    QStandardItem* getItemFromPlugin(const plugin::Plugin* plugin) const;
};

}