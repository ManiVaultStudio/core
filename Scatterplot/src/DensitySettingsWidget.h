#pragma once

#include "WidgetStateMixin.h"

#include <QWidget>

class ScatterplotPlugin;

class QLabel;
class QDoubleSpinBox;
class QSlider;

class DensitySettingsWidget : public QWidget, public WidgetStateMixin
{
public:
    DensitySettingsWidget(QWidget* parent = nullptr);

    void initialize(const ScatterplotPlugin& plugin);

    State getState(const QSize& sourceWidgetSize) const override;

    QIcon getIcon() const override;

protected:

    void updateState() override;

private:
    QLabel*             _label;
    QDoubleSpinBox*     _doubleSpinBox;
    QSlider*            _slider;
};