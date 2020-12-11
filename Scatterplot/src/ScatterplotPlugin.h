#pragma once

#include <ViewPlugin.h>

#include "SelectionTool.h"

#include "widgets/DataSlot.h"
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
class Points;

namespace hdps
{
    class Vector2f;
}

class ScatterplotPlugin : public ViewPlugin
{
    Q_OBJECT
    
public:
    ScatterplotPlugin() :
        ViewPlugin("Scatterplot View"),
        _selectionTool(new SelectionTool(this))
    {
    }

    ~ScatterplotPlugin(void) override;
    
    void init() override;

    void dataAdded(const QString name) Q_DECL_OVERRIDE;
    void dataChanged(const QString name) Q_DECL_OVERRIDE;
    void dataRemoved(const QString name) Q_DECL_OVERRIDE;
    void selectionChanged(const QString dataName) Q_DECL_OVERRIDE;
    DataTypes supportedDataTypes() const Q_DECL_OVERRIDE;
    //void onSelecting(hdps::Selection selection) override;
    //void onSelection(hdps::Selection selection) override;

    ScatterplotWidget* _scatterPlotWidget;
    hdps::DataTypes supportedColorTypes;

    SelectionTool& getSelectionTool();

    bool eventFilter(QObject* target, QEvent* event) override;

    QString getCurrentDataset() const;
    std::uint32_t getNumPoints() const;
    std::uint32_t getNumSelectedPoints() const;

public: // Selection

    void selectAll();
    void clearSelection();
    void invertSelection();

public slots:
    void onDataInput(QString dataSetName);
    void onColorDataInput(QString dataSetName);

protected slots:
    void subsetCreated();

    void xDimPicked(int index);
    void yDimPicked(int index);
    void cDimPicked(int index);

signals:
    void currentDatasetChanged(const QString& datasetName);
    void selectionChanged();

private:
    void updateData();
    void calculatePositions(const Points& points);
    void calculateScalars(std::vector<float>& scalars, const Points& points, int colorIndex);
    void updateSelection();

    //const Points* makeSelection(hdps::Selection selection);

    QString _currentDataSet;
    DataSlot* _dataSlot;
    ScatterplotSettings* _scatterPlotSettings;

    std::vector<hdps::Vector2f> _points;
    unsigned int _numPoints;

    SelectionTool*               _selectionTool;     /** Point selection tool */
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
