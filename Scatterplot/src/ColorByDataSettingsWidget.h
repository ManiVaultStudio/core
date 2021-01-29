#pragma once

#include "widgets/ResponsiveToolBar.h"

class ScatterplotPlugin;

class QLineEdit;
class QPushButton;

class ColorByDataSettingsWidget : public hdps::gui::ResponsiveToolBar::StatefulWidget
{
public:
    ColorByDataSettingsWidget(QWidget* parent = nullptr);

    void initializeUI();
    void setScatterPlotPlugin(const ScatterplotPlugin& plugin);

protected:
    QLayout* getLayout(const hdps::gui::ResponsiveToolBar::WidgetState& state);

private:
    QLineEdit*      _colorDataLineEdit;
    QPushButton*    _removeColorDataPushButton;
};