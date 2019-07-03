#pragma once

#include <ViewPlugin.h>

#include "HeatMapWidget.h"

#include <QList>

using namespace hdps::plugin;

// =============================================================================
// View
// =============================================================================

class PointsPlugin;

class HeatMapPlugin : public ViewPlugin
{
    Q_OBJECT
    
public:
    HeatMapPlugin() : ViewPlugin("Heatmap View") { }
    ~HeatMapPlugin(void) override;
    
    void init() override;

    void dataAdded(const QString name) Q_DECL_OVERRIDE;
    void dataChanged(const QString name) Q_DECL_OVERRIDE;
    void dataRemoved(const QString name) Q_DECL_OVERRIDE;
    void selectionChanged(const QString dataName) Q_DECL_OVERRIDE;
    QStringList supportedDataKinds() Q_DECL_OVERRIDE;
    
protected slots:
    void dataSetPicked(const QString& name);
    void clusterSelected(QList<int> selectedClusters);

private:
    void updateData();

    HeatMapWidget* heatmap;
};


// =============================================================================
// Factory
// =============================================================================

class HeatMapPluginFactory : public ViewPluginFactory
{
    Q_INTERFACES(hdps::plugin::ViewPluginFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "nl.tudelft.HeatMapPlugin"
                      FILE  "HeatMapPlugin.json")
    
public:
    HeatMapPluginFactory(void) {}
    ~HeatMapPluginFactory(void) override {}
    
    ViewPlugin* produce() override;
};
