#pragma once

#include "widgets/ResponsiveToolBar.h"

class ScatterplotPlugin;

class QPushButton;
class QLabel;
class QDoubleSpinBox;
class QSlider;

class PointSettingsWidget : public QStackedWidget
{
public:
    PointSettingsWidget(QWidget* parent = nullptr);

    void initializeUI();
    void setScatterPlotPlugin(const ScatterplotPlugin& plugin);

    QSize sizeHint() const override {
        return currentWidget()->sizeHint();
    }

    QSize minimumSizeHint() const override {
        return currentWidget()->minimumSizeHint();
    }

private:
    hdps::gui::WidgetState          _widgetState;
    hdps::gui::PopupPushButton*     _popupPushButton;
    QWidget*                        _widget;
    QLabel*                         _sizeLabel;
    QDoubleSpinBox*                 _sizeDoubleSpinBox;
    QSlider*                        _sizeSlider;
    QLabel*                         _opacityLabel;
    QDoubleSpinBox*                 _opacityDoubleSpinBox;
    QSlider*                        _opacitySlider;
};