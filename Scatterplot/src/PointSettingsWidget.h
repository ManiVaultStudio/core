#pragma once

#include "ScatterplotSettingsWidget.h"

#include <QStackedWidget>

class QLabel;
class QDoubleSpinBox;
class QSlider;

class PointSettingsWidget : public ScatterplotSettingsWidget
{
public:
    PointSettingsWidget(const hdps::gui::ResponsiveSectionWidget::State& state, QWidget* parent = nullptr);

    void setScatterplotPlugin(ScatterplotPlugin* scatterplotPlugin) override;

private:
    QLabel*             _sizeLabel;
    QDoubleSpinBox*     _sizeDoubleSpinBox;
    QSlider*            _sizeSlider;
    QLabel*             _opacityLabel;
    QDoubleSpinBox*     _opacityDoubleSpinBox;
    QSlider*            _opacitySlider;
};