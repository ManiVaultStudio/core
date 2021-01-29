#pragma once

#include "ScatterplotSettingsWidget.h"

class QLineEdit;
class QPushButton;

class ColorByDataSettingsWidget : public ScatterplotSettingsWidget
{
public:
    ColorByDataSettingsWidget(const hdps::gui::ResponsiveToolBar::WidgetState& state, QWidget* parent = nullptr);

    void setScatterplotPlugin(ScatterplotPlugin* scatterplotPlugin) override;

private:
    QLineEdit*      _colorDataLineEdit;
    QPushButton*    _removeColorDataPushButton;
};