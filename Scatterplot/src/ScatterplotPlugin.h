#pragma once

#include <ViewPlugin.h>

#include "Common.h"

#include "SettingsAction.h"

using namespace hdps::plugin;

class Points;

class PixelSelectionTool;
class ScatterplotWidget;
class DropDataTypesWidget;

namespace hdps
{
    class CoreInterface;
    class Vector2f;

    namespace gui {
        class DropWidget;
    }
}

class ScatterplotPlugin : public ViewPlugin
{
    Q_OBJECT
    
public:
    ScatterplotPlugin();
    ~ScatterplotPlugin() override;
    
    /** Returns the icon of this plugin */
    QIcon getIcon() const override;

    void init() override;

    void onDataEvent(hdps::DataEvent* dataEvent);

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

    PixelSelectionTool* getSelectionTool();

private:

    /** Updates the window title (includes the name of the loaded dataset) */
    void updateWindowTitle();

public: // Data loading

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
    ScatterplotWidget* getScatterplotWidget();
    hdps::CoreInterface* getCore();

private:
    void updateData();
    void calculatePositions(const Points& points);
    void calculateScalars(std::vector<float>& scalars, const Points& points, int colorIndex);
    void updateSelection();
    
private:
    QString                         _currentDataSet;
    QString                         _currentColorDataSet;
    std::vector<hdps::Vector2f>     _points;
    unsigned int                    _numPoints;
    
    
protected:
    PixelSelectionTool*         _pixelSelectionTool;
    ScatterplotWidget*          _scatterPlotWidget;
    hdps::gui::DropWidget*      _dropWidget;
    SettingsAction              _settingsAction;
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
