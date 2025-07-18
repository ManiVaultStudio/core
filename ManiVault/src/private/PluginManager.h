// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <models/PluginsTreeModel.h>

#include <AbstractPluginManager.h>
#include <PluginFactory.h>

namespace mv {

using namespace plugin;

class PluginManager final : public AbstractPluginManager
{
public:

    /**
     * Construct manager with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    PluginManager(QObject* parent);

    /** Reset when destructed */
    ~PluginManager() override;
    
    /** Perform manager startup initialization */
    void initialize() override;

    /** Resets the contents of the plugin manager */
    void reset() override;

    /** Loads all plugin factories from the plugin directory */
    void loadPluginFactories() override;

    /**
     * Determine whether a plugin of \p kind is loaded
     * @param kind Plugin kind
     * @return Boolean determining whether a plugin of \p kind is loaded
     */
    bool isPluginLoaded(const QString& kind) const override;

public: // Plugin creation/destruction

    /**
     * Create a plugin of \p kind with input \p datasets
     * @param kind Kind of plugin (name of the plugin)
     * @param inputDatasets Zero or more datasets upon which the plugin is based (e.g. analysis plugin)
     * @param outputDatasets Zero or more datasets that the plugin produces (e.g. analysis plugin)
     * @return Pointer to created plugin, nullptr if creation failed
     */
    plugin::Plugin* requestPlugin(const QString& kind, Datasets inputDatasets = Datasets(), Datasets outputDatasets = Datasets()) override;
    
    /**
     * Create a view plugin of \p kind, dock it to \p dockToViewPlugin at \p dockArea and assign the \p datasets
     * @param kind Kind of plugin (name of the plugin)
     * @param dockToViewPlugin View plugin instance to dock to
     * @param dockArea Dock area to dock in
     * @param datasets Datasets to assign to the view plugin
     * @return Pointer of view plugin type to created view plugin, nullptr if creation failed
     */
    plugin::ViewPlugin* requestViewPlugin(const QString& kind, plugin::ViewPlugin* dockToViewPlugin = nullptr, gui::DockAreaFlag dockArea = gui::DockAreaFlag::Right, Datasets datasets = Datasets()) override;

    /**
     * Create a view plugin of \p kind, assign the \p datasets and float it on top of the main window
     * @param kind Kind of plugin (name of the plugin)
     * @param datasets Datasets to assign to the view plugin
     * @return Pointer of view plugin type to created view plugin, nullptr if creation failed
     */
    plugin::ViewPlugin* requestViewPluginFloated(const QString& kind, Datasets datasets = Datasets()) override;

private:

    /**
     * Add \p plugin to the data manager
     * @param plugin Pointer to the plugin to add
     */
    void addPlugin(plugin::Plugin* plugin) override;

public: // Destroy

    /**
     * Destroy \p plugin
     * @param plugin Pointer to the plugin that is to be destroyed
     */
    void destroyPlugin(plugin::Plugin* plugin) override;

    /**
     * Destroy plugin by \p pluginId
     * @param pluginId Globally unique identifier of the plugin that is to be destroyed
     */
    void destroyPluginById(const QString& pluginId) override;

public: // Plugin factory

    /**
     * Get plugin factory from \p pluginKind
     * @param pluginKind Kind of plugin
     * @return Plugin factory of \p pluginKind, nullptr if not found
     */
    plugin::PluginFactory* getPluginFactory(const QString& pluginKind) const override;

    /**
     * Get plugin factories for \p pluginType
     * @param pluginType Plugin type
     * @return Vector of pointers to plugin factories
     */
    std::vector<plugin::PluginFactory*> getPluginFactoriesByType(const plugin::Type& pluginType) const override;

    /**
     * Get plugin factories for \p pluginTypes (by default it gets all plugins factories for all types)
     * @param pluginTypes Plugin types
     * @return Vector of pointers to plugin factories of \p pluginTypes
     */
    std::vector<plugin::PluginFactory*> getPluginFactoriesByTypes(const plugin::Types& pluginTypes = plugin::Types{ plugin::Type::ANALYSIS, plugin::Type::DATA, plugin::Type::LOADER, plugin::Type::WRITER, plugin::Type::TRANSFORMATION, plugin::Type::VIEW }) const override;

    /**
     * Get plugin instances for \p pluginFactory
     * @param pluginFactory Pointer to plugin factory
     * @return Vector of shared pointers to plugin instances
     */
    std::vector<plugin::Plugin*> getPluginsByFactory(const plugin::PluginFactory* pluginFactory) const override;

public: // Plugin getters

    /**
     * Get plugin instances for \p pluginType (by default it gets all plugins for all types)
     * @param pluginType Plugin type
     * @return Vector of shared pointers to plugin instances
     */
    std::vector<plugin::Plugin*> getPluginsByType(const plugin::Type& pluginType) const override;

    /**
     * Get plugin instances for \p pluginTypes
     * @param pluginTypes Plugin types
     * @return Vector of shared pointers to plugin instances of \p pluginTypes
     */
    std::vector<plugin::Plugin*> getPluginsByTypes(const plugin::Types& pluginTypes = plugin::Types{ plugin::Type::ANALYSIS, plugin::Type::DATA, plugin::Type::LOADER, plugin::Type::WRITER, plugin::Type::TRANSFORMATION, plugin::Type::VIEW }) const override;

    /**
     * Get plugin kinds by \p pluginType
     * @param pluginTypes Plugin type(s), all plugin types if empty
     * @return List of plugin kinds
     */
    QStringList getPluginKindsByPluginTypes(const plugin::Types& pluginTypes = plugin::Types{ plugin::Type::ANALYSIS, plugin::Type::DATA, plugin::Type::LOADER, plugin::Type::WRITER, plugin::Type::TRANSFORMATION, plugin::Type::VIEW }) const;

    /**
     * Get loaded plugin kinds by \p pluginType
     * @param pluginTypes Plugin type(s), all plugin types if empty
     * @return List of loaded plugin kinds
     */
    QStringList getLoadedPluginKinds(const plugin::Types& pluginTypes = plugin::Types{ plugin::Type::ANALYSIS, plugin::Type::DATA, plugin::Type::LOADER, plugin::Type::WRITER, plugin::Type::TRANSFORMATION, plugin::Type::VIEW }) const override;

    /**
     * Get used plugin kinds by \p pluginType
     * @param pluginTypes Plugin type(s), all plugin types if empty
     * @return List of used plugin kinds
     */
    QStringList getUsedPluginKinds(const plugin::Types& pluginTypes = plugin::Types{ plugin::Type::ANALYSIS, plugin::Type::DATA, plugin::Type::LOADER, plugin::Type::WRITER, plugin::Type::TRANSFORMATION, plugin::Type::VIEW }) const override;

public: // Plugin trigger actions

    /**
     * Get plugin trigger actions by \p pluginType
     * @param pluginType Type of plugin e.g. analysis, exporter
     * @return Vector of plugin trigger actions
     */
    gui::PluginTriggerActions getPluginTriggerActions(const plugin::Type& pluginType) const override;

    /**
     * Get plugin trigger actions by \p pluginType and \p datasets
     * @param pluginType Type of plugin e.g. analysis, exporter
     * @param datasets Vector of input datasets
     * @return Vector of plugin trigger actions
     */
    gui::PluginTriggerActions getPluginTriggerActions(const plugin::Type& pluginType, const Datasets& datasets) const override;

    /**
     * Get plugin trigger actions by \p pluginType and \p dataTypes
     * @param pluginType Type of plugin e.g. analysis, exporter
     * @param dataTypes Vector of input data types
     * @return Vector of plugin trigger actions
     */
    gui::PluginTriggerActions getPluginTriggerActions(const plugin::Type& pluginType, const DataTypes& dataTypes) const override;

    /**
     * Get plugin trigger actions by \p pluginKind and \p datasets
     * @param pluginKind Kind of plugin
     * @param datasets Vector of input datasets
     * @return Vector of plugin trigger actions
     */
    gui::PluginTriggerActions getPluginTriggerActions(const QString& pluginKind, const Datasets& datasets) const override;

    /**
     * Get plugin trigger actions by \p pluginKind and \p dataTypes
     * @param pluginKind Kind of plugin
     * @param dataTypes Vector of input data types
     * @return Vector of plugin trigger actions
     */
    gui::PluginTriggerActions getPluginTriggerActions(const QString& pluginKind, const DataTypes& dataTypes) const override;

public: // Plugin query

    /**
     * Get plugin GUI name from plugin kind
     * @param pluginKind Kind of plugin
     * @param GUI name of the plugin, empty if the plugin kind was not found
     */
    QString getPluginGuiName(const QString& pluginKind) const;

    /**
     * Get plugin icon from plugin kind
     * @param pluginKind Kind of plugin
     * @return Plugin icon name of the plugin, null icon the plugin kind was not found
     */
    QIcon getPluginIcon(const QString& pluginKind) const;

public: // Serialization

    /**
     * Load widget action from variant
     * @param Variant representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant
     * @return Variant representation of the widget action
     */
    QVariantMap toVariantMap() const override;

public: // Model access

    /**
     * Get list model of all loaded plugins
     * @return Reference to plugins list model
     */
    const PluginsListModel& getListModel() const override;

    /**
     * Get tree model of all loaded plugins
     * @return Reference to plugins tree model
     */
    const PluginsTreeModel& getTreeModel() const override;

protected:

    /**
     * Resolves plugin dependencies, returns list of resolved plugin filenames
     * @param pluginDir Plugin scan directory
     * @return List of resolved plugin filenames
     */
    QStringList resolveDependencies(QDir pluginDir) const override;

private:
    QHash<QString, PluginFactory*>                  _pluginFactories;   /** All loaded plugin factories */
    std::vector<std::unique_ptr<plugin::Plugin>>    _plugins;           /** Vector of plugin instances */
    PluginsListModel*                               _listModel;         /** List model of all loaded plugins */
    PluginsTreeModel*                               _treeModel;         /** Tree model of all loaded plugins */
};

}
