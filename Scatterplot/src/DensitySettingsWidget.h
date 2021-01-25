#pragma once

#include "widgets/ResponsiveToolBar.h"

class ScatterplotPlugin;

class QLabel;
class QDoubleSpinBox;
class QSlider;

class DensitySettingsWidget : public hdps::gui::ResponsiveToolBar::StatefulWidget
{
public:
    DensitySettingsWidget(QWidget* parent = nullptr);

    void initializeUI();
    void setScatterPlotPlugin(const ScatterplotPlugin& plugin);

protected:
    QLayout* getLayout(const hdps::gui::ResponsiveToolBar::WidgetState& state) override;

private:
    QLabel*             _label;
    QDoubleSpinBox*     _doubleSpinBox;
    QSlider*            _slider;
};