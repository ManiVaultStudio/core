#pragma once

#include "widgets/ResponsiveToolBar.h"

#include <QStackedWidget>

class ScatterplotPlugin;

class PointSettingsWidget;
class DensitySettingsWidget;

class PlotSettingsWidget : public hdps::gui::ResponsiveToolBar::StatefulWidget
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
    PlotSettingsWidget(QWidget* parent = nullptr);

    void initializeUI();
    void setScatterPlotPlugin(const ScatterplotPlugin& plugin);

    void setState(const hdps::gui::ResponsiveToolBar::WidgetState& state) override;

protected:
    QLayout* getLayout(const hdps::gui::ResponsiveToolBar::WidgetState& state) override;

private:
    StackedWidget*              _stackedWidget;
    PointSettingsWidget*        _pointSettingsWidget;
    DensitySettingsWidget*      _densitySettingsWidget;
};