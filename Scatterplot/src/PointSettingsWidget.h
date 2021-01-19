#pragma once

#include "widgets/ResponsiveToolBar.h"

class ScatterplotPlugin;

class QLabel;
class QDoubleSpinBox;
class QSlider;

class PointSettingsWidget : public hdps::gui::ResponsiveToolBar::Widget
{
public:
    PointSettingsWidget(QWidget* parent = nullptr);

    void initialize(const ScatterplotPlugin& plugin);

    void updateState() override;

private:
    QLabel*             _sizeLabel;
    QDoubleSpinBox*     _sizeDoubleSpinBox;
    QSlider*            _sizeSlider;
    QLabel*             _opacityLabel;
    QDoubleSpinBox*     _opacityDoubleSpinBox;
    QSlider*            _opacitySlider;
};