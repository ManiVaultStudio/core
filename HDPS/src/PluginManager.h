#pragma once

#include <AbstractPluginManager.h>
#include <PluginFactory.h>

#include <actions/TriggerAction.h>
#include <actions/ToggleAction.h>

namespace hdps {

using namespace plugin;
using namespace gui;

class PluginManager : public AbstractPluginManager
{

    Q_OBJECT

public:

    /** Default constructor */
    PluginManager();

    /** Default destructor */
    ~PluginManager() override;
    
    /** Loads all plugin factories from the plugin directory and adds them as menu items */
    void loadPlugins();

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
    PluginType* requestPlugin(const QString& kind, const Datasets& datasets)
    {
        return dynamic_cast<PluginType*>(createPlugin(kind, datasets));
    }

    /**
     * Get plugin kinds by plugin type(s)
     * @param pluginTypes Plugin type(s)
     * @return Plugin kinds
     */
    QStringList getPluginKindsByPluginTypes(const plugin::Types& pluginTypes) const;

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
     * Get a list of plugin kinds (names) given a plugin type
     * @param pluginType Type of plugin e.g. analysis, exporter
     * @return List of compatible plugin kinds that can handle the data type
     */
    QStringList requestPluginKindsByPluginType(const plugin::Type& pluginType);

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

protected:

    /**
     * Resolves plugin dependencies, returns list of resolved plugin filenames
     * @param pluginDir Plugin scan directory
     * @return List of resolved plugin filenames
     */
    QStringList resolveDependencies(QDir pluginDir) const;

signals:

    /**
     * Signals that an action has become available that triggers plugin help
     * @param pluginTriggerHelpAction Action that triggers plugin help
     */
    void addPluginTriggerHelpAction(TriggerAction& pluginTriggerHelpAction);

    /**
     * Signals that a trigger action has become available that triggers an import plugin
     * @param pluginTriggerHelpAction Action that triggers the import plugin
     */
    void addImportPluginTriggerAction(TriggerAction& pluginTriggerAction);

    /**
     * Signals that a trigger action has become available that triggers a view plugin
     * @param pluginTriggerHelpAction Action that triggers the view plugin
     */
    void addViewPluginTriggerAction(TriggerAction& pluginTriggerAction);

    /**
     * Signals that a toggle action has become available that toggles view plugin visibility
     * @param viewPluginVisibleAction Action that toggles the view plugin visibility
     */
    void addViewPluginVisibleAction(ToggleAction& viewPluginVisibleAction);

private:
    QHash<QString, PluginFactory*> _pluginFactories;
};

}
