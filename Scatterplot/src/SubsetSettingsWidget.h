#pragma once

#include "ScatterplotSettingsWidget.h"

class ScatterplotPlugin;

class QPushButton;
class QCheckBox;

class SubsetSettingsWidget : public ScatterplotSettingsWidget
{
public:
    SubsetSettingsWidget(const hdps::gui::ResponsiveToolBar::WidgetState& state, QWidget* parent = nullptr);

    void initializeUI();
    void setScatterPlotPlugin(const ScatterplotPlugin& plugin);

private:
    QPushButton*    _createSubsetPushButton;
    QCheckBox*      _fromSourceCheckBox;
};