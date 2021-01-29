#pragma once

#include "ScatterplotSettingsWidget.h"

class QComboBox;

class ColorByDimensionSettingsWidget : public ScatterplotSettingsWidget
{
public:
    ColorByDimensionSettingsWidget(const hdps::gui::ResponsiveToolBar::WidgetState& state, QWidget* parent = nullptr);

    QComboBox* getColorDimensionComboBox() {
        return _colorDimensionComboBox;
    }

protected:
    void connectToPlugin() override;

private:
    QComboBox*  _colorDimensionComboBox;
};