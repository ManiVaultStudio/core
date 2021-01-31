#pragma once

#include "ScatterplotSettingsWidget.h"

class QLabel;
class QDoubleSpinBox;
class QSlider;

class DensitySettingsWidget : public ScatterplotSettingsWidget
{
public:
    DensitySettingsWidget(const hdps::gui::ResponsiveSectionWidget::State& state, QWidget* parent = nullptr);

    void setScatterplotPlugin(ScatterplotPlugin* scatterplotPlugin) override;

private:
    QLabel*             _label;
    QDoubleSpinBox*     _doubleSpinBox;
    QSlider*            _slider;
};