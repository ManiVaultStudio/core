#pragma once

#include "ScatterplotSettingsWidget.h"

#include <QStackedWidget>

class QLabel;
class QDoubleSpinBox;
class QSlider;

class PointSettingsWidget : public ScatterplotSettingsWidget
{
public:
    PointSettingsWidget(const hdps::gui::ResponsiveToolBar::WidgetState& state, QWidget* parent = nullptr);

protected:
    void connectToPlugin() override;

private:
    QLabel*             _sizeLabel;
    QDoubleSpinBox*     _sizeDoubleSpinBox;
    QSlider*            _sizeSlider;
    QLabel*             _opacityLabel;
    QDoubleSpinBox*     _opacityDoubleSpinBox;
    QSlider*            _opacitySlider;
};