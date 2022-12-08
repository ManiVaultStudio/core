#pragma once

#include "AbstractManager.h"

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

class AbstractPluginManager : public AbstractManager
{
    Q_OBJECT

public:

    /** Aliases */
    using UniquePtrPlugin   = std::unique_ptr<plugin::Plugin>;
    using UniquePtrsPlugin  = std::vector<UniquePtrPlugin>;
    using PluginPtrs        = std::vector<plugin::Plugin*>;

public:
    
    /** Loads all plugin factories from the plugin directory and adds them as menu items */
    virtual void loadPlugins() = 0;

    /**
     * Determine whether a plugin of \p kind is loaded
     * @param kind Plugin kind
     * @return Boolean determining whether a plugin of \p kind is loaded
     */
    virtual bool isPluginLoaded(const QString& kind) const = 0;

    /**
    * Creates a new plugin instance of the given kind and adds it to the core
    * @param kind Kind of plugin
    * @param datasets Zero or more datasets upon which the plugin is based (e.g. analysis plugin)
    * @return Pointer to created plugin
    */
    virtual plugin::Plugin* createPlugin(const QString& kind, const Datasets& datasets = Datasets()) = 0;
    
    /**
     * Create a plugin of \p kind
     * @param kind Kind of plugin (name of the plugin)
     * @return Pointer to created plugin
     */
    template<typename PluginType>
    PluginType* requestPlugin(const QString& kind, const Datasets& datasets)
    {
        return dynamic_cast<PluginType*>(createPlugin(kind, datasets));
    }

    /**
     * Get plugin instances for \p pluginType
     * @param pluginType Plugin type
     * @return Vector of pointers to plugin instances
     */
    virtual PluginPtrs getPluginsByType(const plugin::Type& pluginType) const = 0;

    /**
     * Get plugin kinds by plugin type(s)
     * @param pluginTypes Plugin type(s)
     * @return Plugin kinds
     */
    virtual QStringList getPluginKindsByPluginTypes(const plugin::Types& pluginTypes) const = 0;

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

    /**
     * Get plugin GUI name from plugin kind
     * @param pluginKind Kind of plugin
     * @param GUI name of the plugin, empty if the plugin kind was not found
     */
    virtual QString getPluginGuiName(const QString& pluginKind) const = 0;

    /**
     * Get a list of plugin kinds (names) given a plugin type
     * @param pluginType Type of plugin e.g. analysis, exporter
     * @return List of compatible plugin kinds that can handle the data type
     */
    virtual QStringList requestPluginKindsByPluginType(const plugin::Type& pluginType) = 0;

    /**
     * Get plugin icon from plugin kind
     * @param pluginKind Kind of plugin
     * @return Plugin icon name of the plugin, null icon if the plugin kind was not found
     */
    virtual QIcon getPluginIcon(const QString& pluginKind) const = 0;

protected: // Factory

    /**
     * Get plugin factory from \p pluginKind
     * @param pluginKind Kind of plugin
     * @return Plugin factory of \p pluginKind, nullptr if not found
     */
    virtual plugin::PluginFactory* getPluginFactory(const QString& pluginKind) const = 0;

protected:

    /**
     * Resolves plugin dependencies, returns list of resolved plugin filenames
     * @param pluginDir Plugin scan directory
     * @return List of resolved plugin filenames
     */
    virtual QStringList resolveDependencies(QDir pluginDir) const = 0;

signals:

    /**
     * Signals that a plugin factory was loaded
     * @param pluginFactory Pointer to plugin factory that was loaded
     */
    void pluginFactoryLoaded(plugin::PluginFactory* pluginFactory);

    /**
     * Signals that \p plugin instance is added to the plugin manager
     * @param plugin Pointer to the plugin that was added
     */
    void pluginAdded(plugin::Plugin* plugin);

    /**
     * Signals that \p plugin instance is about to be removed from the plugin manager
     * @param plugin Pointer to the plugin that is about to be removed
     */
    void pluginAboutToBeRemoved(plugin::Plugin* plugin);

    friend class gui::PluginTriggerAction;
};

}
