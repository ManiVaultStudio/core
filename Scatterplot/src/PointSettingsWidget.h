#pragma once

#include "widgets/ResponsiveToolBar.h"

#include <QStackedWidget>

class ScatterplotPlugin;

class QPushButton;
class QLabel;
class QDoubleSpinBox;
class QSlider;

class PointSettingsWidget : public hdps::gui::ResponsiveToolBar::StatefulWidget
{
public:
    PointSettingsWidget(QWidget* parent = nullptr);

    void initializeUI();
    void setScatterPlotPlugin(const ScatterplotPlugin& plugin);

protected:
    QLayout* getLayout(const hdps::gui::ResponsiveToolBar::WidgetState& state);
    QSize getSizeHint(const hdps::gui::ResponsiveToolBar::WidgetState& state);

private:
    ScatterplotPlugin*  _scatterplotPlugin;
    QLabel*             _sizeLabel;
    QDoubleSpinBox*     _sizeDoubleSpinBox;
    QSlider*            _sizeSlider;
    QLabel*             _opacityLabel;
    QDoubleSpinBox*     _opacityDoubleSpinBox;
    QSlider*            _opacitySlider;
};