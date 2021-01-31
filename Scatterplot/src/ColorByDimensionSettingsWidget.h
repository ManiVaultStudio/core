#pragma once

#include "ScatterplotSettingsWidget.h"

class QComboBox;

class ColorByDimensionSettingsWidget : public ScatterplotSettingsWidget
{
public:
    ColorByDimensionSettingsWidget(const hdps::gui::ResponsiveSectionWidget::State& state, QWidget* parent = nullptr);

    QComboBox* getColorDimensionComboBox() {
        return _colorDimensionComboBox;
    }

    void setScatterplotPlugin(ScatterplotPlugin* scatterplotPlugin) override;

private:
    QComboBox*  _colorDimensionComboBox;
};