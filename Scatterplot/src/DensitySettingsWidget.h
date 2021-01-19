#pragma once

#include "widgets/ResponsiveToolBar.h"

class ScatterplotPlugin;

class QLabel;
class QDoubleSpinBox;
class QSlider;

class DensitySettingsWidget : public hdps::gui::ResponsiveToolBar::Widget
{
public:
    DensitySettingsWidget(QWidget* parent = nullptr);

    void initialize(const ScatterplotPlugin& plugin);

protected:

    void updateState() override;

private:
    QLabel*             _label;
    QDoubleSpinBox*     _doubleSpinBox;
    QSlider*            _slider;
};