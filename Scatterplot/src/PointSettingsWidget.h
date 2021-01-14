#pragma once

#include "WidgetStateMixin.h"

#include <QWidget>

class ScatterplotPlugin;

class QLabel;
class QDoubleSpinBox;
class QSlider;

class PointSettingsWidget : public QWidget, public WidgetStateMixin
{
public:
    PointSettingsWidget(const WidgetStateMixin::State& state, QWidget* parent = nullptr);

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