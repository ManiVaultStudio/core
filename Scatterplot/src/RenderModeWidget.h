#pragma once

#include "widgets/ResponsiveToolBar.h"

class ScatterplotPlugin;

class QPushButton;

class RenderModeWidget : public hdps::gui::ResponsiveToolBar::StatefulWidget
{
public:
    RenderModeWidget(QWidget* parent = nullptr);

    void initializeUI();
    void setScatterPlotPlugin(const ScatterplotPlugin& plugin);

protected:
    QLayout* getLayout(const hdps::gui::ResponsiveToolBar::WidgetState& state) override;

private:
    QPushButton*    _scatterPlotPushButton;
    QPushButton*    _densityPlotPushButton;
    QPushButton*    _contourPlotPushButton;
};