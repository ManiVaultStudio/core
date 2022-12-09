#pragma once

#include <AbstractPluginManager.h>
#include <PluginFactory.h>

namespace hdps {

using namespace plugin;

class PluginManager final : public AbstractPluginManager
{
public:

    /** Default constructor */
    PluginManager();

    /** Default destructor */
    ~PluginManager() override;
    
    /** Resets the contents of the plugin manager */
    void reset() override;

    /** Loads all plugin factories from the plugin directory and adds them as menu items */
    void loadPlugins();

    /**
     * Determine whether a plugin of \p kind is loaded
     * @param kind Plugin kind
     * @return Boolean determining whether a plugin of \p kind is loaded
     */
    bool isPluginLoaded(const QString& kind) const override;

    /**
    * Creates a new plugin instance of the given kind and adds it to the core
    * @param kind Kind of plugin
    * @param datasets Zero or more datasets upon which the plugin is based (e.g. analysis plugin)
    * @return Pointer to created plugin
    */
    plugin::Plugin* createPlugin(const QString& kind, const Datasets& datasets = Datasets());
    
    /**
     * Create a plugin of \p kind
     * @param kind Kind of plugin (name of the plugin)
     * @return Pointer to created plugin
     */
    template<typename PluginType>
    PluginType* createPlugin(const QString& kind, const Datasets& datasets)
    {
        return dynamic_cast<PluginType*>(createPlugin(kind, datasets));
    }

    /**
     * Destroy \p plugin
     * @param plugin Pointer to the plugin that is to be destroyed
     */
    void destroyPlugin(plugin::Plugin* plugin) override;

    /**
     * Get plugin factories for \p pluginType
     * @param pluginType Plugin type
     * @return Vector of pointers to plugin factories
     */
    PluginFactoryPtrs getPluginFactoriesByType(const plugin::Type& pluginType) const override;

    /**
     * Get plugin factories for \p pluginTypes (by default it gets all plugins factories for all types)
     * @param pluginTypes Plugin types
     * @return Vector of pointers to plugin factories of \p pluginTypes
     */
    PluginFactoryPtrs getPluginFactoriesByTypes(const plugin::Types& pluginTypes = plugin::Types{ plugin::Type::ANALYSIS, plugin::Type::DATA, plugin::Type::LOADER, plugin::Type::WRITER, plugin::Type::TRANSFORMATION, plugin::Type::VIEW }) const override;

    /**
     * Get plugin instances for \p pluginFactory
     * @param pluginFactory Pointer to plugin factory
     * @return Vector of pointers to plugin instances
     */
    PluginPtrs getPluginsByFactory(const plugin::PluginFactory* pluginFactory) const override;

    /**
     * Get plugin instances for \p pluginType (by default it gets all plugins for all types)
     * @param pluginType Plugin type
     * @return Vector of pointers to plugin instances
     */
    PluginPtrs getPluginsByType(const plugin::Type& pluginType) const override;

    /**
     * Get plugin instances for \p pluginTypes
     * @param pluginTypes Plugin types
     * @return Vector of pointers to plugin instances of \p pluginTypes
     */
    PluginPtrs getPluginsByTypes(const plugin::Types& pluginTypes = plugin::Types{ plugin::Type::ANALYSIS, plugin::Type::DATA, plugin::Type::LOADER, plugin::Type::WRITER, plugin::Type::TRANSFORMATION, plugin::Type::VIEW }) const override;

    /**
     * Get plugin kinds by plugin type(s)
     * @param pluginTypes Plugin type(s)
     * @return Plugin kinds
     */
    QStringList getPluginKindsByPluginTypes(const plugin::Types& pluginTypes) const;

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
    gui::PluginTriggerActions getPluginTriggerActions(const plugin::Type& pluginType, const Datasets& datasets) const;

    /**
     * Get plugin trigger actions by \p pluginType and \p dataTypes
     * @param pluginType Type of plugin e.g. analysis, exporter
     * @param dataTypes Vector of input data types
     * @return Vector of plugin trigger actions
     */
    gui::PluginTriggerActions getPluginTriggerActions(const plugin::Type& pluginType, const DataTypes& dataTypes) const;

    /**
     * Get plugin trigger actions by \p pluginKind and \p datasets
     * @param pluginKind Kind of plugin
     * @param datasets Vector of input datasets
     * @return Vector of plugin trigger actions
     */
    gui::PluginTriggerActions getPluginTriggerActions(const QString& pluginKind, const Datasets& datasets) const;

    /**
     * Get plugin trigger actions by \p pluginKind and \p dataTypes
     * @param pluginKind Kind of plugin
     * @param dataTypes Vector of input data types
     * @return Vector of plugin trigger actions
     */
    gui::PluginTriggerActions getPluginTriggerActions(const QString& pluginKind, const DataTypes& dataTypes) const;

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

protected: // Factory

    /**
     * Get plugin factory from \p pluginKind
     * @param pluginKind Kind of plugin
     * @return Plugin factory of \p pluginKind, nullptr if not found
     */
    plugin::PluginFactory* getPluginFactory(const QString& pluginKind) const override;

protected:

    /**
     * Resolves plugin dependencies, returns list of resolved plugin filenames
     * @param pluginDir Plugin scan directory
     * @return List of resolved plugin filenames
     */
    QStringList resolveDependencies(QDir pluginDir) const;

private:
    QHash<QString, PluginFactory*>  _pluginFactories;   /**  */
    QList<plugin::Plugin*>          _plugins;           /** List of plugin instances currently present in the application. Instances are stored by type. */
};

}
