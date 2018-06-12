#ifndef SCATTERPLOT_PLUGIN_H
#define SCATTERPLOT_PLUGIN_H

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
class hdps::Vector2f;

class ScatterplotPlugin : public ViewPlugin, public SelectionListener
{
    Q_OBJECT
    
public:
    ScatterplotPlugin()
    :
        ViewPlugin("Scatterplot View")
    { }
    ~ScatterplotPlugin(void);
    
    void init();

    unsigned int pointSize() const;
    unsigned int sigma() const;

    void dataAdded(const QString name) Q_DECL_OVERRIDE;
    void dataChanged(const QString name) Q_DECL_OVERRIDE;
    void dataRemoved(const QString name) Q_DECL_OVERRIDE;
    void selectionChanged(const QString dataName) Q_DECL_OVERRIDE;
    QStringList supportedDataKinds() Q_DECL_OVERRIDE;
    virtual void onSelecting(hdps::Selection selection);
    virtual void onSelection(hdps::Selection selection);

    ScatterplotWidget* _scatterPlotWidget;

protected slots:
    void dataSetPicked(const QString& name);

    void subsetCreated();

    void xDimPicked(int index);
    void yDimPicked(int index);

private:
    void updateData();
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
    ~ScatterplotPluginFactory(void) {}
    
    ViewPlugin* produce();
};

#endif // SCATTERPLOT_PLUGIN_H
