#pragma once

#include "ResponsiveSectionWidget.h"

#include "graphics/Vector3f.h"

#include <QWidget>

class ScatterplotPlugin;

namespace Ui {
    class SettingsWidget;
}

class RenderModeWidget;
class PlotSettingsWidget;
class DimensionSettingsWidget;
class SubsetSettingsWidget;

class SettingsWidget : public QWidget
{
    Q_OBJECT

public:
    SettingsWidget(const ScatterplotPlugin& plugin);
    ~SettingsWidget() override;

    int getXDimension();
    int getYDimension();

    hdps::Vector3f getBaseColor();
    hdps::Vector3f getSelectionColor();

    void initDimOptions(const unsigned int nDim);
    void initDimOptions(const std::vector<QString>& dimNames);
    void initScalarDimOptions(const unsigned int nDim);
    void initScalarDimOptions(const std::vector<QString>& dimNames);

    void paintEvent(QPaintEvent* event) override;

private:
    std::unique_ptr<Ui::SettingsWidget>                 _ui;                            /** Externally loaded UI */
    ResponsiveSectionWidget<RenderModeWidget>*          _renderModeWidget;              /** Widget for picking a render mode (scatter plot, density and contour map) */
    ResponsiveSectionWidget<PlotSettingsWidget>*        _plotSettinsWidget;             /** Widget for plot settings (depends on the render mode) */
    ResponsiveSectionWidget<DimensionSettingsWidget>*   _dimensionSettinsWidget;        /** Widget for selection position and color dimensions */
    ResponsiveSectionWidget<SubsetSettingsWidget>*      _subsetSettingsWidget;          /** Widget for creating subsets from selected points */
    hdps::Vector3f                                      _baseColor;                     /** Base color */
    hdps::Vector3f                                      _selectionColor;                /** Selection color */

    static const hdps::Vector3f DEFAULT_BASE_COLOR;
    static const hdps::Vector3f DEFAULT_SELECTION_COLOR;
};