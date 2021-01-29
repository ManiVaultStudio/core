#pragma once

#include "widgets/ResponsiveToolBar.h"

class ScatterplotPlugin;

class QComboBox;

class ColorByDimensionSettingsWidget : public hdps::gui::ResponsiveToolBar::StatefulWidget
{
public:
    ColorByDimensionSettingsWidget(QWidget* parent = nullptr);

    void initializeUI();
    void setScatterPlotPlugin(const ScatterplotPlugin& plugin);

    QComboBox* getColorDimensionComboBox() {
        return _colorDimensionComboBox;
    }

protected:
    QLayout* getLayout(const hdps::gui::ResponsiveToolBar::WidgetState& state);

private:
    QComboBox*  _colorDimensionComboBox;
};