#pragma once

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
    PointSettingsWidget*        _pointSettingsWidget;
    DensitySettingsWidget*      _densitySettingsWidget;
};