#pragma once

#include <ViewPlugin.h>
#include "SelectionListener.h"

#include "ScatterplotWidget.h"

#include <QComboBox>
#include <QSlider>
#include <QPushButton>
#include <QRectF>

using namespace hdps::plugin;

// =============================================================================
// View
// =============================================================================

class ScatterplotSettings;
class PointsPlugin;
class IndexSet;

namespace hdps
{
    class Vector2f;
}

class ScatterplotPlugin : public ViewPlugin, public SelectionListener
{
    Q_OBJECT
    
public:
    ScatterplotPlugin()
    :
        ViewPlugin("Scatterplot View")
    { }
    ~ScatterplotPlugin(void) override;
    
    void init() override;

    void dataAdded(const QString name) Q_DECL_OVERRIDE;
    void dataChanged(const QString name) Q_DECL_OVERRIDE;
    void dataRemoved(const QString name) Q_DECL_OVERRIDE;
    void selectionChanged(const QString dataName) Q_DECL_OVERRIDE;
    QStringList supportedDataKinds() Q_DECL_OVERRIDE;
    void onSelecting(hdps::Selection selection) override;
    void onSelection(hdps::Selection selection) override;

    ScatterplotWidget* _scatterPlotWidget;

protected slots:
    void dataSetPicked(const QString& name);

    void subsetCreated();

    void xDimPicked(int index);
    void yDimPicked(int index);
    void cDimPicked(int index);

private:
    void updateData();
    void calculatePositions(const IndexSet& dataSet);
    void calculateScalars(std::vector<float>& scalars, const PointsPlugin& data);
    void updateSelection();

    void makeSelection(hdps::Selection selection);

    ScatterplotSettings* settings;

    std::vector<hdps::Vector2f> _points;
    unsigned int _numPoints;
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
    ~ScatterplotPluginFactory(void) override {}
    
    ViewPlugin* produce() override;
};
