// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractManager.h"

#include "actions/PluginTriggerAction.h"

#include "util/DockArea.h"

#include <QObject>
#include <QString>
#include <QStringList>

namespace mv
{

class PluginsListModel;
class PluginsTreeModel;

namespace plugin {
    class Plugin;
    class PluginFactory;
    class ViewPlugin;
}

/**
 * Abstract plugin manager
 *
 * Base abstract plugin manager class for managing plugin instances.
 *
 * @ingroup mv_managers
 * @author Thomas Kroes
 */
class CORE_EXPORT AbstractPluginManager : public AbstractManager
{
    Q_OBJECT

public:

    /**
     * Construct manager with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    AbstractPluginManager(QObject* parent) :
        AbstractManager(parent, "Plugins")
    {
    }

    /** Loads all plugin factories from the plugin directory */
    virtual void loadPluginFactories() = 0;

    /**
     * Determine whether a plugin of \p kind is loaded
     * @param kind Plugin kind
     * @return Boolean determining whether a plugin of \p kind is loaded
     */
    virtual bool isPluginLoaded(const QString& kind) const = 0;

public: // Plugin creation/destruction

    /**
     * Create a plugin of \p kind with input \p datasets
     * @param kind Kind of plugin (name of the plugin)
     * @param datasets Zero or more datasets upon which the plugin is based (e.g. analysis plugin)
     * @return Pointer to created plugin, nullptr if creation failed
     */
    virtual plugin::Plugin* requestPlugin(const QString& kind, Datasets inputDatasets = Datasets(), Datasets outputDatasets = Datasets()) = 0;

    /**
     * Create a plugin of \p kind with \p inputDatasets
     * @param kind Kind of plugin (name of the plugin)
     * @param datasets Zero or more datasets upon which the plugin is based (e.g. analysis plugin)
     * @return Pointer of \p PluginType to created plugin, nullptr if creation failed
     */
    template<typename PluginType>
    PluginType* requestPlugin(const QString& kind, Datasets inputDatasets = Datasets(), Datasets outputDatasets = Datasets())
    {
        return dynamic_cast<PluginType*>(requestPlugin(kind, inputDatasets, outputDatasets));
    }

private:

    /**
     * Add \p plugin to the data manager
     * @param plugin Pointer to the plugin to add
     */
    virtual void addPlugin(plugin::Plugin* plugin) = 0;

public:

    /**
     * Create a view plugin of \p kind, dock it to \p dockToViewPlugin at \p dockArea and assign the \p datasets
     * @param kind Kind of plugin (name of the plugin)
     * @param dockToViewPlugin View plugin instance to dock to
     * @param dockArea Dock area to dock in
     * @param datasets Datasets to assign to the view plugin
     * @return Pointer of view plugin type to created view plugin, nullptr if creation failed
     */
    virtual plugin::ViewPlugin* requestViewPlugin(const QString& kind, plugin::ViewPlugin* dockToViewPlugin = nullptr, gui::DockAreaFlag dockArea = gui::DockAreaFlag::Right, Datasets datasets = Datasets()) = 0;

    /**
     * Create a view plugin of \p kind and float it on top of the main window
     * @param kind Kind of plugin (name of the plugin)
     * @param datasets Datasets to assign to the view plugin
     * @return Pointer of view plugin type to created view plugin, nullptr if creation failed
     */
    virtual plugin::ViewPlugin* requestViewPluginFloated(const QString& kind, Datasets datasets = Datasets()) = 0;

public: // Destroy

    /**
     * Destroy \p plugin
     * @param plugin Pointer to the plugin that is to be destroyed
     */
    virtual void destroyPlugin(plugin::Plugin* plugin) = 0;

    /**
     * Destroy plugin by \p pluginId
     * @param pluginId Globally unique identifier of the plugin that is to be destroyed
     */
    virtual void destroyPluginById(const QString& pluginId) = 0;

public: // Plugin factory

    /**
     * Get plugin factory from \p pluginKind
     * @param pluginKind Kind of plugin
     * @return Plugin factory of \p pluginKind, nullptr if not found
     */
    virtual plugin::PluginFactory* getPluginFactory(const QString& pluginKind) const = 0;

    /**
     * Get plugin factories for \p pluginType
     * @param pluginType Plugin type
     * @return Vector of pointers to plugin factories
     */
    virtual std::vector<plugin::PluginFactory*> getPluginFactoriesByType(const plugin::Type& pluginType) const = 0;

    /**
     * Get plugin factories for \p pluginTypes (by default it gets all plugins factories for all types)
     * @param pluginTypes Plugin types
     * @return Vector of pointers to plugin factories of \p pluginTypes
     */
    virtual std::vector<plugin::PluginFactory*> getPluginFactoriesByTypes(const plugin::Types& pluginTypes = plugin::Types{ plugin::Type::ANALYSIS, plugin::Type::DATA, plugin::Type::LOADER, plugin::Type::WRITER, plugin::Type::TRANSFORMATION, plugin::Type::VIEW }) const = 0;

    /**
     * Get plugin instances for \p pluginFactory
     * @param pluginFactory Pointer to plugin factory
     * @return Vector of pointers to plugin instances
     */
    virtual std::vector<plugin::Plugin*> getPluginsByFactory(const plugin::PluginFactory* pluginFactory) const = 0;

public: // Plugin getters

    /**
     * Get plugin instances for \p pluginType
     * @param pluginType Plugin type
     * @return Vector of pointers to plugin instances
     */
    virtual std::vector<plugin::Plugin*> getPluginsByType(const plugin::Type& pluginType) const = 0;

    /**
     * Get plugin instances for \p pluginTypes (by default it gets all plugins for all types)
     * @param pluginTypes Plugin types
     * @return Vector of pointers to plugin instances of \p pluginTypes
     */
    virtual std::vector<plugin::Plugin*> getPluginsByTypes(const plugin::Types& pluginTypes = plugin::Types{ plugin::Type::ANALYSIS, plugin::Type::DATA, plugin::Type::LOADER, plugin::Type::WRITER, plugin::Type::TRANSFORMATION, plugin::Type::VIEW }) const = 0;

    /**
     * Get plugin kinds by \p pluginType
     * @param pluginTypes Plugin type(s), all plugin types if empty
     * @return List of plugin kinds
     */
    virtual QStringList getPluginKindsByPluginTypes(const plugin::Types& pluginTypes = plugin::Types{ plugin::Type::ANALYSIS, plugin::Type::DATA, plugin::Type::LOADER, plugin::Type::WRITER, plugin::Type::TRANSFORMATION, plugin::Type::VIEW }) const = 0;

    /**
     * Get loaded plugin kinds by \p pluginType
     * @param pluginTypes Plugin type(s), all plugin types if empty
     * @return List of loaded plugin kinds
     */
    virtual QStringList getLoadedPluginKinds(const plugin::Types& pluginTypes = plugin::Types{ plugin::Type::ANALYSIS, plugin::Type::DATA, plugin::Type::LOADER, plugin::Type::WRITER, plugin::Type::TRANSFORMATION, plugin::Type::VIEW }) const = 0;

    /**
     * Get used plugin kinds by \p pluginType
     * @param pluginTypes Plugin type(s), all plugin types if empty
     * @return List of used plugin kinds
     */
    virtual QStringList getUsedPluginKinds(const plugin::Types& pluginTypes = plugin::Types{ plugin::Type::ANALYSIS, plugin::Type::DATA, plugin::Type::LOADER, plugin::Type::WRITER, plugin::Type::TRANSFORMATION, plugin::Type::VIEW }) const = 0;

public: // Plugin trigger actions

    /**
     * Get plugin trigger actions by \p pluginType
     * @param pluginType Type of plugin e.g. analysis, exporter
     * @return Vector of plugin trigger actions
     */
    virtual gui::PluginTriggerActions getPluginTriggerActions(const plugin::Type& pluginType) const = 0;

    /**
     * Get plugin trigger actions by \p pluginType and \p datasets
     * @param pluginType Type of plugin e.g. analysis, exporter
     * @param datasets Vector of input datasets
     * @return Vector of plugin trigger actions
     */
    virtual gui::PluginTriggerActions getPluginTriggerActions(const plugin::Type& pluginType, const Datasets& datasets) const = 0;

    /**
     * Get plugin trigger actions by \p pluginType and \p dataTypes
     * @param pluginType Type of plugin e.g. analysis, exporter
     * @param dataTypes Vector of input data types
     * @return Vector of plugin trigger actions
     */
    virtual gui::PluginTriggerActions getPluginTriggerActions(const plugin::Type& pluginType, const DataTypes& dataTypes) const = 0;

    /**
     * Get plugin trigger actions by \p pluginKind and \p datasets
     * @param pluginKind Kind of plugin
     * @param datasets Vector of input datasets
     * @return Vector of plugin trigger actions
     */
    virtual gui::PluginTriggerActions getPluginTriggerActions(const QString& pluginKind, const Datasets& datasets) const = 0;

    /**
     * Get plugin trigger actions by \p pluginKind and \p dataTypes
     * @param pluginKind Kind of plugin
     * @param dataTypes Vector of input data types
     * @return Vector of plugin trigger actions
     */
    virtual gui::PluginTriggerActions getPluginTriggerActions(const QString& pluginKind, const DataTypes& dataTypes) const = 0;

public: // Plugin query

    /**
     * Get plugin GUI name from plugin kind
     * @param pluginKind Kind of plugin
     * @param GUI name of the plugin, empty if the plugin kind was not found
     */
    virtual QString getPluginGuiName(const QString& pluginKind) const = 0;

    /**
     * Get plugin icon from plugin kind
     * @param pluginKind Kind of plugin
     * @return Plugin icon name of the plugin, null icon if the plugin kind was not found
     */
    virtual QIcon getPluginIcon(const QString& pluginKind) const = 0;

public: // Model access

    /**
     * Get list model of all loaded plugins
     * @return Reference to plugins list model
     */
    virtual const PluginsListModel& getListModel() const = 0;

    /**
     * Get tree model of all loaded plugins
     * @return Reference to plugins tree model
     */
    virtual const PluginsTreeModel& getTreeModel() const = 0;

protected:

    /**
     * Resolves plugin dependencies, returns list of resolved plugin filenames
     * @param pluginDir Plugin scan directory
     * @return List of resolved plugin filenames
     */
    virtual QStringList resolveDependencies(QDir pluginDir) const = 0;

signals:

    /**
     * Signals that \p plugin instance is added to the plugin manager
     * @param plugin Pointer to the plugin that was added
     */
    void pluginAdded(plugin::Plugin* plugin);

    /**
     * Signals that \p plugin is about to be destroyed by the plugin manager
     * @param plugin Pointer to the plugin that is about to be destroyed
     */
    void pluginAboutToBeDestroyed(plugin::Plugin* plugin);

    /**
     * Signals that plugin with \p id is destroyed by the plugin manager
     * @param id Globally unique ID of the destroyed plugin
     */
    void pluginDestroyed(const QString& id);

    /** Signals that all plugin factories have been loaded */
    void pluginFactoriesLoaded();

    friend class gui::PluginTriggerAction;
};

}
