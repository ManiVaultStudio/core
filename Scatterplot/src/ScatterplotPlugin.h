#pragma once

#include <ViewPlugin.h>

#include "Common.h"

#include "widgets/DataSlot.h"
#include "PixelSelectionTool.h"

#include "ScatterplotWidget.h"
#include "SettingsAction.h"

using namespace hdps::plugin;

// =============================================================================
// View
// =============================================================================

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
    ~ScatterplotPlugin() override;
    
    /** Returns the icon of this plugin */
    QIcon getIcon() const override {
        return Application::getIconFont("FontAwesome").getIcon("braille");
    }

    void init() override;

    void onDataEvent(DataEvent* dataEvent);

    QString getCurrentDataset() const;
    std::uint32_t getNumberOfPoints() const;
    std::uint32_t getNumberOfSelectedPoints() const;

public:
    void createSubset(const bool& fromSourceData = false, const QString& name = "");

public: // Dimension picking
    void setXDimension(const std::int32_t& dimensionIndex);
    void setYDimension(const std::int32_t& dimensionIndex);
    void setColorDimension(const std::int32_t& dimensionIndex);

public: // Selection
    bool canSelect() const;
    bool canSelectAll() const;
    bool canClearSelection() const;
    bool canInvertSelection() const;
    void selectAll();
    void clearSelection();
    void invertSelection();

    PixelSelectionTool& getSelectionTool();

private:

    /** Updates the window title (includes the name of the loaded dataset) */
    void updateWindowTitle();

protected: // Data loading

    /**
     * Load point data
     * @param dataSetName Name of the point dataset
     */
    void loadPointData(const QString& dataSetName);

    /**
     * Load color data
     * @param dataSetName Name of the color/cluster dataset
     */
    void loadColorData(const QString& dataSetName);

    void selectPoints();

signals:
    void currentDatasetChanged(const QString& datasetName);
    void selectionChanged();

public:

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
    DataSlot*                       _dataSlot;
    std::vector<hdps::Vector2f>     _points;
    unsigned int                    _numPoints;
    PixelSelectionTool*             _pixelSelectionTool;        /** Pixel selection tool */
    
protected:// Widgets
    ScatterplotWidget*              _scatterPlotWidget;
    SettingsAction                  _settingsAction;
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
