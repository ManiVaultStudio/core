#pragma once

#include "ScatterplotSettingsWidget.h"

class QLabel;
class QDoubleSpinBox;
class QSlider;

class DensitySettingsWidget : public ScatterplotSettingsWidget
{
public:
    DensitySettingsWidget(const hdps::gui::ResponsiveToolBar::WidgetState& state, QWidget* parent = nullptr);

protected:
    void connectToPlugin() override;

private:
    QLabel*             _label;
    QDoubleSpinBox*     _doubleSpinBox;
    QSlider*            _slider;
};