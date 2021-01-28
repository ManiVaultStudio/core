#pragma once

#include "widgets/ResponsiveToolBar.h"

#include <QStackedWidget>

class ScatterplotPlugin;

class ColorByDimensionSettingsWidget;
class ColorByDataSettingsWidget;

class QLabel;
class QComboBox;

class ColorSettingsWidget : public hdps::gui::ResponsiveToolBar::StatefulWidget
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
    ColorSettingsWidget(QWidget* parent = nullptr);

    void initializeUI();
    void setScatterPlotPlugin(const ScatterplotPlugin& plugin);

    void setState(const hdps::gui::ResponsiveToolBar::WidgetState& state, const bool& forceUpdate = false) override;

public:
    void setScalarDimensions(unsigned int numDimensions, const std::vector<QString>& names = std::vector<QString>());

protected:
    QLayout* getLayout(const hdps::gui::ResponsiveToolBar::WidgetState& state) override;

private:
    QLabel*                             _colorByLabel;
    QComboBox*                          _colorByComboBox;
    StackedWidget*                      _stackedWidget;
    ColorByDimensionSettingsWidget*     _colorByDimensionSettingsWidget;
    ColorByDataSettingsWidget*          _colorByDataSettingsWidget;
};