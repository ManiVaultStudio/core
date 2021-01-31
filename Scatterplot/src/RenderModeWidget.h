#pragma once

#include "ScatterplotSettingsWidget.h"

class QPushButton;

class RenderModeWidget : public ScatterplotSettingsWidget
{
public:
    RenderModeWidget(const hdps::gui::ResponsiveSectionWidget::State& state, QWidget* parent = nullptr);

    void setScatterplotPlugin(ScatterplotPlugin* scatterplotPlugin) override;

private:
    QPushButton*    _scatterPlotPushButton;
    QPushButton*    _densityPlotPushButton;
    QPushButton*    _contourPlotPushButton;
};