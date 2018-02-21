#ifndef DENSITY_PLOT_PLUGIN_H
#define DENSITY_PLOT_PLUGIN_H

#include <ViewPlugin.h>
#include "widgets/DensityPlotWidget.h"

#include <QComboBox>
#include <QSlider>
#include <QPushButton>

using namespace hdps::plugin;

// =============================================================================
// View
// =============================================================================

class DensityPlotSettings;
class PointsPlugin;

class DensityPlotPlugin : public ViewPlugin, public SelectionListener
{
    Q_OBJECT
    
public:
    DensityPlotPlugin()
    :
        ViewPlugin("Density Plot")
    { }
    ~DensityPlotPlugin(void);
    
    void init();

    void dataAdded(const QString name) Q_DECL_OVERRIDE;
    void dataChanged(const QString name) Q_DECL_OVERRIDE;
    void dataRemoved(const QString name) Q_DECL_OVERRIDE;
    void selectionChanged(const QString dataName) Q_DECL_OVERRIDE;
    QStringList supportedDataKinds() Q_DECL_OVERRIDE;
    virtual void onSelection(const std::vector<unsigned int> selection) const;
protected slots:
    void dataSetPicked(const QString& name);
    void subsetCreated();

    void xDimPicked(int index);
    void yDimPicked(int index);
private:
    void updateData();
    float getMaxLength(const std::vector<float>* data, const int nDim) const;

    hdps::gui::DensityPlotWidget* _widget;

    DensityPlotSettings* _settings;
};


// =============================================================================
// Factory
// =============================================================================

class DensityPlotPluginFactory : public ViewPluginFactory
{
    Q_INTERFACES(hdps::plugin::ViewPluginFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "nl.tudelft.DensityPlotPlugin"
                      FILE  "DensityPlotPlugin.json")
    
public:
    DensityPlotPluginFactory(void) {}
    ~DensityPlotPluginFactory(void) {}
    
    ViewPlugin* produce();
};

#endif // DENSITY_PLOT_PLUGIN_H
