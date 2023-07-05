// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractManager.h"
#include "Plugin.h"
#include "PluginFactory.h"
#include "ViewPlugin.h"

#include "actions/PluginTriggerAction.h"

#include <QObject>
#include <QString>
#include <QStringList>
#include <QDir>

namespace hdps
{

namespace plugin {
    class PluginFactory;
}

/**
 * Abstract plugin manager
 *
 * Base abstract plugin manager class for managing plugin instances.
 *
 * @author Thomas Kroes
 */
class AbstractPluginManager : public AbstractManager
{
    Q_OBJECT

public:

    /**
     * Construct plugin manager with \p parent object
     * @param parent Pointer to parent object
     */
    AbstractPluginManager(QObject* parent = nullptr) :
        AbstractManager(parent, "Plugins")
    {
    }
    
    /** Loads all plugin factories from the plugin directory and adds them as menu items */
    virtual void loadPlugins() = 0;

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
     * @return Pointer to created plugin (nullptr if creation failed)
     */
    virtual plugin::Plugin* requestPlugin(const QString& kind, Datasets datasets = Datasets()) = 0;

    /**
     * Create a plugin of \p kind with \p inputDatasets
     * @param kind Kind of plugin (name of the plugin)
     * @param datasets Zero or more datasets upon which the plugin is based (e.g. analysis plugin)
     * @return Pointer to created plugin of plugin type (nullptr if creation failed)
     */
    template<typename PluginType>
    PluginType* requestPlugin(const QString& kind, Datasets datasets = Datasets())
    {
        return dynamic_cast<PluginType*>(requestPlugin(kind, datasets));
    }

    /**
     * Create a view plugin plugin of \p kind and dock it to \p dockToViewPlugin at \p dockArea
     * @param kind Kind of plugin (name of the plugin)
     * @param dockToViewPlugin View plugin instance to dock to
     * @param dockArea Dock area to dock in
     * @return Pointer to created view plugin (nullptr if creation failed)
     */
    virtual plugin::ViewPlugin* requestViewPlugin(const QString& kind, plugin::ViewPlugin* dockToViewPlugin = nullptr, gui::DockAreaFlag dockArea = gui::DockAreaFlag::Right, Datasets datasets = Datasets()) = 0;

    /**
     * Destroy \p plugin
     * @param plugin Pointer to the plugin that is to be destroyed
     */
    virtual void destroyPlugin(plugin::Plugin* plugin) = 0;
    
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
    virtual plugin::PluginFactoryPtrs getPluginFactoriesByType(const plugin::Type& pluginType) const = 0;

    /**
     * Get plugin factories for \p pluginTypes (by default it gets all plugins factories for all types)
     * @param pluginTypes Plugin types
     * @return Vector of pointers to plugin factories of \p pluginTypes
     */
    virtual plugin::PluginFactoryPtrs getPluginFactoriesByTypes(const plugin::Types& pluginTypes = plugin::Types{ plugin::Type::ANALYSIS, plugin::Type::DATA, plugin::Type::LOADER, plugin::Type::WRITER, plugin::Type::TRANSFORMATION, plugin::Type::VIEW }) const = 0;

    /**
     * Get plugin instances for \p pluginFactory
     * @param pluginFactory Pointer to plugin factory
     * @return Vector of pointers to plugin instances
     */
    virtual plugin::PluginPtrs getPluginsByFactory(const plugin::PluginFactory* pluginFactory) const = 0;

public: // Plugin getters

    /**
     * Get plugin instances for \p pluginType
     * @param pluginType Plugin type
     * @return Vector of pointers to plugin instances
     */
    virtual plugin::PluginPtrs getPluginsByType(const plugin::Type& pluginType) const = 0;

    /**
     * Get plugin instances for \p pluginTypes (by default it gets all plugins for all types)
     * @param pluginTypes Plugin types
     * @return Vector of pointers to plugin instances of \p pluginTypes
     */
    virtual plugin::PluginPtrs getPluginsByTypes(const plugin::Types& pluginTypes = plugin::Types{ plugin::Type::ANALYSIS, plugin::Type::DATA, plugin::Type::LOADER, plugin::Type::WRITER, plugin::Type::TRANSFORMATION, plugin::Type::VIEW }) const = 0;

    /**
     * Get plugin kinds by plugin type(s)
     * @param pluginTypes Plugin type(s)
     * @return Plugin kinds
     */
    virtual QStringList getPluginKindsByPluginTypes(const plugin::Types& pluginTypes) const = 0;

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

    friend class gui::PluginTriggerAction;
};

}
