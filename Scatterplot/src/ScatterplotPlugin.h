#pragma once

#include <ViewPlugin.h>

#include "Common.h"

#include "widgets/DataSlot.h"
#include "PixelSelectionTool.h"
#include "ScatterplotWidget.h"

using namespace hdps::plugin;

// =============================================================================
// View
// =============================================================================

class QToolBar;

class Points;
class SettingsWidget;

namespace hdps
{
    class Vector2f;
}

class ScatterplotPlugin : public ViewPlugin
{
    Q_OBJECT
    
public:
    ScatterplotPlugin();

    ~ScatterplotPlugin(void) override;
    
    /** Returns the icon of this plugin */
    QIcon getIcon() const override {
        return Application::getIconFont("FontAwesome").getIcon("braille");
    }

    /** Returns the toolbar which is shown at the top of the view plugin (dock) widget */
    QToolBar* getToolBar();

    void init() override;

    void dataAdded(const QString name) Q_DECL_OVERRIDE;
    void dataChanged(const QString name) Q_DECL_OVERRIDE;
    void dataRemoved(const QString name) Q_DECL_OVERRIDE;
    void selectionChanged(const QString dataName) Q_DECL_OVERRIDE;
    DataTypes supportedDataTypes() const Q_DECL_OVERRIDE;

    PixelSelectionTool& getSelectionTool();

    bool eventFilter(QObject* target, QEvent* event) override;

    QString getCurrentDataset() const;
    std::uint32_t getNumPoints() const;
    std::uint32_t getNumSelectedPoints() const;

public: // Selection

    bool canSelect() const;
    bool canSelectAll() const;
    bool canClearSelection() const;
    bool canInvertSelection() const;

    void selectAll();
    void clearSelection();
    void invertSelection();

public slots:
    void onDataInput(QString dataSetName);
    void onColorDataInput(QString dataSetName);

protected slots:
    void subsetCreated();

public:
    void xDimPicked(int index);
    void yDimPicked(int index);
    void cDimPicked(int index);

    void selectPoints();

signals:
    void currentDatasetChanged(const QString& datasetName);
    void selectionChanged();

public:

    /** Returns the supported drop color types */
    hdps::DataTypes getSupportedColorTypes() const {
        return _supportedColorTypes;
    }

    /** Returns the scatter plot widget */
    ScatterplotWidget* getScatterplotWidget() {
        return _scatterPlotWidget;
    }

private:
    void updateData();
    void calculatePositions(const Points& points);
    void calculateScalars(std::vector<float>& scalars, const Points& points, int colorIndex);
    void updateSelection();

private:
    QString                         _currentDataSet;
    hdps::DataTypes                 _supportedColorTypes;
    DataSlot*                       _dataSlot;
    std::vector<hdps::Vector2f>     _points;
    unsigned int                    _numPoints;
    PixelSelectionTool*             _pixelSelectionTool;        /** Pixel selection tool */
    ScatterplotWidget*              _scatterPlotWidget;         /**  */
    SettingsWidget*                 _settingsWidget;
    QToolBar*                       _toolBar;
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
