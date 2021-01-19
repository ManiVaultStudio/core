#pragma once

#include "widgets/ResponsiveToolBar.h"

class ScatterplotPlugin;

class QPushButton;

class RenderModeWidget : public hdps::gui::ResponsiveToolBar::Widget
{
public:
    RenderModeWidget(QWidget* parent = nullptr);

    void initialize(const ScatterplotPlugin& plugin);
    
    void updateState() override;

private:
    QPushButton*    _scatterPlotPushButton;
    QPushButton*    _densityPlotPushButton;
    QPushButton*    _contourPlotPushButton;
};