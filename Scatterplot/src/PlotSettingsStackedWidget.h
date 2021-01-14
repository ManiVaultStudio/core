#pragma once

#include "StateWidget.h"

#include <QStackedWidget>

class ScatterplotPlugin;
class PointSettingsWidget;
class DensitySettingsWidget;

class PlotSettingsStackedWidget : public QStackedWidget
{
public:
    PlotSettingsStackedWidget(QWidget* parent = nullptr);

    void initialize(const ScatterplotPlugin& plugin);

    QSize sizeHint() const override {
        return currentWidget()->sizeHint();
    }

    QSize minimumSizeHint() const override {
        return currentWidget()->minimumSizeHint();
    }

    PointSettingsWidget* getPointSettingsWidget();
    DensitySettingsWidget* getDensitySettingsWidget();

private:
    StateWidget<PointSettingsWidget>*       _pointSettingsWidget;
    StateWidget<DensitySettingsWidget>*     _densitySettingsWidget;
};