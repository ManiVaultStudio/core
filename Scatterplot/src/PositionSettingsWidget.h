#pragma once

#include "ScatterplotSettingsWidget.h"

class ScatterplotPlugin;

class QLabel;
class QComboBox;

class PositionSettingsWidget : public ScatterplotSettingsWidget
{
public:
    PositionSettingsWidget(const hdps::gui::ResponsiveToolBar::WidgetState& state, QWidget* parent = nullptr);

    void initializeUI();
    void setScatterPlotPlugin(const ScatterplotPlugin& plugin);

public:

    int getDimensionX();
    int getDimensionY();

    void setDimensions(unsigned int numDimensions, const std::vector<QString>& names = std::vector<QString>());

private:
    ScatterplotPlugin*  _scatterplotPlugin;
    QLabel*             _xDimensionLabel;
    QComboBox*          _xDimensionComboBox;
    QLabel*             _yDimensionLabel;
    QComboBox*          _yDimensionComboBox;
};