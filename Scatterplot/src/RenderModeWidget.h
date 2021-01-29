#pragma once

#include "ScatterplotSettingsWidget.h"

class QPushButton;

class RenderModeWidget : public ScatterplotSettingsWidget
{
public:
    RenderModeWidget(const hdps::gui::ResponsiveToolBar::WidgetState& state, QWidget* parent = nullptr);

protected:
    void connectToPlugin() override;

private:
    QPushButton*    _scatterPlotPushButton;
    QPushButton*    _densityPlotPushButton;
    QPushButton*    _contourPlotPushButton;
};