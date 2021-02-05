#pragma once

#include "ScatterplotSettingsWidget.h"

#include <QStackedWidget>

class PointSettingsWidget;
class DensitySettingsWidget;

class PlotSettingsWidget : public ScatterplotSettingsWidget
{
public:
    class StackedWidget : public QStackedWidget {
    public:
        QSize sizeHint() const override {
            return currentWidget()->sizeHint();
        }

        QSize minimumSizeHint() const override {
            return currentWidget()->minimumSizeHint();
        }
    };
    
public:
    PlotSettingsWidget(const hdps::gui::ResponsiveSectionWidget::WidgetState& widgetState, QWidget* parent = nullptr);

    void setScatterplotPlugin(ScatterplotPlugin* scatterplotPlugin) override;

private:
    StackedWidget*              _stackedWidget;
    PointSettingsWidget*        _pointSettingsWidget;
    DensitySettingsWidget*      _densitySettingsWidget;
};