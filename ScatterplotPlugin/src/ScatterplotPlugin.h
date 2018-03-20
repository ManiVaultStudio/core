#ifndef SCATTERPLOT_PLUGIN_H
#define SCATTERPLOT_PLUGIN_H

#include <ViewPlugin.h>
#include "widgets/ScatterplotWidget.h"

#include <QComboBox>
#include <QSlider>
#include <QPushButton>

using namespace hdps::plugin;

// =============================================================================
// View
// =============================================================================

class ScatterplotSettings;
class PointsPlugin;

class ScatterplotPlugin : public ViewPlugin, public SelectionListener
{
    Q_OBJECT
    
public:
    ScatterplotPlugin()
    :
        ViewPlugin("Scatterplot View"),
        _pointSize(10)
    { }
    ~ScatterplotPlugin(void);
    
    void init();

    unsigned int pointSize() const;

    void dataAdded(const QString name) Q_DECL_OVERRIDE;
    void dataChanged(const QString name) Q_DECL_OVERRIDE;
    void dataRemoved(const QString name) Q_DECL_OVERRIDE;
    void selectionChanged(const QString dataName) Q_DECL_OVERRIDE;
    QStringList supportedDataKinds() Q_DECL_OVERRIDE;
    virtual void onSelection(const std::vector<unsigned int> selection) const;
protected slots:
    void dataSetPicked(const QString& name);
    void pointSizeChanged(const int size);
    void subsetCreated();
    void renderModePicked(const int index);

    void xDimPicked(int index);
    void yDimPicked(int index);
private:
    void updateData();
    float getMaxLength(const std::vector<float>* data, const int nDim) const;

    hdps::gui::ScatterplotWidget* _scatterPlotWidget;

    ScatterplotSettings* settings;

    unsigned int _pointSize;
};


// =============================================================================
// Factory
// =============================================================================

class ScatterplotPluginFactory : public ViewPluginFactory
{
    Q_INTERFACES(hdps::plugin::ViewPluginFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "nl.tudelft.ScatterplotPlugin"
                      FILE  "ScatterplotPlugin.json")
    
public:
    ScatterplotPluginFactory(void) {}
    ~ScatterplotPluginFactory(void) {}
    
    ViewPlugin* produce();
};

#endif // SCATTERPLOT_PLUGIN_H
