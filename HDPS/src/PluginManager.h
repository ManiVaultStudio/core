#pragma once

#include "Core.h"

#include "PluginFactory.h"

#include <actions/WidgetAction.h>

#include <QObject>
#include <QString>
#include <QStringList>
#include <QHash>
#include <QDir>

namespace Ui
{
    class MainWindow;
}

namespace hdps
{
namespace plugin
{

class PluginManager : public WidgetAction
{
public:
    PluginManager(Core& core);
    ~PluginManager(void) override;
    
    /**
    * Loads all plugin factories from the plugin directory and adds them as menu items.
    */
    void loadPlugins();

    /**
    * Creates a new plugin instance of the given kind and adds it to the core
    * @param kind Kind of plugin
    * @param datasetTypes Sequence of dataset types for which this plugin producer is valid
    * @param datasets Input datasets
    */
    QString createPlugin(const QString& kind, const QStringList& datasetTypes =  QStringList(), const Datasets& datasets = Datasets());
    
    /**
     * Create an analysis plugin for one one or more datasets
     * @param kind Kind of analysis plugin
     * @param datasets Datasets to analyze
     */
    void createAnalysisPlugin(const QString& kind, Datasets datasets);

    /**
     * Create an exporter plugin for one one or more datasets
     * @param kind Kind of exporter plugin
     * @param datasets Datasets to export
     */
    void createExporterPlugin(const QString& kind, Datasets datasets);

    /**
     * Create a view plugin for one one or more datasets
     * @param kind Kind of view plugin
     * @param datasets Input dataset(s)
     */
    void createViewPlugin(const QString& kind, const Datasets& datasets);

    /**
     * Create a transformation plugin for one one or more datasets
     * @param kind Kind of transformation plugin
     * @param datasets Input dataset(s)
     */
    void createTransformationPlugin(const QString& kind, const Datasets& datasets);

    /**
     * Get plugin actions by plugin type and data types (of the selected datasets in the data hierarchy)
     * @param pluginType Type of plugin e.g. analysis, exporter
     * @param datasets Selected dataset(s)
     * @return Vector of plugin producer actions
     */
    QList<QAction*> getPluginActionsByPluginTypeAndDatasets(const plugin::Type& pluginType, const Datasets& datasets) const;

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

private:

    /**
    * Resolves plugin dependencies, returns list of resolved plugin filenames.
    */
    QStringList resolveDependencies(QDir pluginDir) const;

    Core& _core;

    QHash<QString, PluginFactory*> _pluginFactories;

private slots:
    QString pluginTriggered(const QString& kind);
};

}

}
