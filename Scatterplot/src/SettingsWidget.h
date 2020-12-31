#pragma once

#include "ResponsiveSectionWidget.h"

#include "graphics/Vector3f.h"

#include <QWidget>

class ScatterplotPlugin;

class RenderModeWidget;
class PlotSettingsWidget;
class DimensionSettingsWidget;
class SubsetSettingsWidget;

namespace Ui {
    class SettingsWidget;
}

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
    hdps::Vector3f                          _baseColor;         /** Base color */
    hdps::Vector3f                          _selectionColor;    /** Selection color */
    std::unique_ptr<Ui::SettingsWidget>     _ui;                /** Externally loaded UI */

    ResponsiveSectionWidget<RenderModeWidget>*          _renderModeWidget;
    ResponsiveSectionWidget<PlotSettingsWidget>*        _plotSettinsWidget;
    ResponsiveSectionWidget<DimensionSettingsWidget>*   _dimensionSettinsWidget;
    ResponsiveSectionWidget<SubsetSettingsWidget>*      _subsetSettingsWidget;

    static const hdps::Vector3f DEFAULT_BASE_COLOR;
    static const hdps::Vector3f DEFAULT_SELECTION_COLOR;
};