#ifndef CLUSTERING_PLUGIN_H
#define CLUSTERING_PLUGIN_H

#include <AnalysisPlugin.h>

#include "ClusteringSettingsWidget.h"

using namespace hdps::plugin;
using namespace hdps::gui;

// =============================================================================
// View
// =============================================================================

class ClusteringPlugin : public QObject, public AnalysisPlugin
{
    Q_OBJECT   
public:
    ClusteringPlugin() : AnalysisPlugin("Clustering Analysis") { }
    ~ClusteringPlugin(void);
    
    void init();

    void dataAdded(const QString name) Q_DECL_OVERRIDE;
    void dataChanged(const QString name) Q_DECL_OVERRIDE;
    void dataRemoved(const QString name) Q_DECL_OVERRIDE;
    void selectionChanged(const QString dataName) Q_DECL_OVERRIDE;
    QStringList supportedDataKinds() Q_DECL_OVERRIDE;

    SettingsWidget* const getSettings();
public slots:
    void dataSetPicked(const QString& name);

    void startComputation();

private:
    std::unique_ptr<ClusteringSettingsWidget> _settings;
};

// =============================================================================
// Factory
// =============================================================================

class ClusteringPluginFactory : public AnalysisPluginFactory
{
    Q_INTERFACES(hdps::plugin::AnalysisPluginFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "nl.tudelft.ClusteringPlugin"
                      FILE  "ClusteringPlugin.json")
    
public:
    ClusteringPluginFactory(void) {}
    ~ClusteringPluginFactory(void) {}
    
    AnalysisPlugin* produce();
};

#endif // CLUSTERING_PLUGIN_H
