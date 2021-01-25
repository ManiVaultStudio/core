#pragma once

#include "widgets/ResponsiveToolBar.h"

class ScatterplotPlugin;

class PointSettingsWidget;
class DensitySettingsWidget;

class PlotSettingsWidget : public hdps::gui::ResponsiveToolBar::StatefulWidget
{
public:
    PlotSettingsWidget(QWidget* parent = nullptr);

    void initializeUI();
    void setScatterPlotPlugin(const ScatterplotPlugin& plugin);

protected:

    QIcon getIcon() const override;
    QLayout* getLayout(const hdps::gui::ResponsiveToolBar::WidgetState& state) override;

private:
    PointSettingsWidget*        _pointSettingsWidget;
    DensitySettingsWidget*      _densitySettingsWidget;
};