#ifndef HEAT_MAP_PLUGIN_H
#define HEAT_MAP_PLUGIN_H

#include <ViewPlugin.h>

#include "HeatMapWidget.h"

using namespace hdps::plugin;

// =============================================================================
// View
// =============================================================================

class PointsPlugin;

class HeatMapPlugin : public ViewPlugin, public SelectionListener
{
    Q_OBJECT
    
public:
    HeatMapPlugin() : ViewPlugin("Heatmap View") { }
    ~HeatMapPlugin(void);
    
    void init();

    void dataAdded(const QString name) Q_DECL_OVERRIDE;
    void dataChanged(const QString name) Q_DECL_OVERRIDE;
    void dataRemoved(const QString name) Q_DECL_OVERRIDE;
    void selectionChanged(const QString dataName) Q_DECL_OVERRIDE;
    QStringList supportedDataKinds() Q_DECL_OVERRIDE;
    virtual void onSelection(const std::vector<unsigned int> selection) const;
protected slots:
    void dataSetPicked(const QString& name);

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
    ~HeatMapPluginFactory(void) {}
    
    ViewPlugin* produce();
};

#endif // HEAT_MAP_PLUGIN_H
