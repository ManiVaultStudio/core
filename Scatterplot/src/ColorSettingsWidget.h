#pragma once

#include "ScatterplotSettingsWidget.h"

#include <QStackedWidget>

class ScatterplotPlugin;

class ColorByDimensionSettingsWidget;
class ColorByDataSettingsWidget;

class QLabel;
class QComboBox;

class ColorSettingsWidget : public ScatterplotSettingsWidget
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
    ColorSettingsWidget(const hdps::gui::ResponsiveToolBar::WidgetState& state, QWidget* parent = nullptr);

    void initializeUI();
    void setScatterPlotPlugin(const ScatterplotPlugin& plugin);

public:
    void setScalarDimensions(unsigned int numDimensions, const std::vector<QString>& names = std::vector<QString>());

private:
    ScatterplotPlugin*                  _scatterplotPlugin;
    QLabel*                             _colorByLabel;
    QComboBox*                          _colorByComboBox;
    StackedWidget*                      _stackedWidget;
    ColorByDimensionSettingsWidget*     _colorByDimensionSettingsWidget;
    ColorByDataSettingsWidget*          _colorByDataSettingsWidget;
};