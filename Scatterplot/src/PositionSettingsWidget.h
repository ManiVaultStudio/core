#pragma once

#include "widgets/ResponsiveToolBar.h"

class ScatterplotPlugin;

class QLabel;
class QComboBox;

class PositionSettingsWidget : public hdps::gui::ResponsiveToolBar::StatefulWidget
{
public:
    PositionSettingsWidget(QWidget* parent = nullptr);

    void initializeUI();
    void setScatterPlotPlugin(const ScatterplotPlugin& plugin);

public:

    int getDimensionX();
    int getDimensionY();

    void setDimensions(unsigned int numDimensions, const std::vector<QString>& names = std::vector<QString>());

protected:
    QLayout* getLayout(const hdps::gui::ResponsiveToolBar::WidgetState& state) override;

private:
    QLabel*         _xDimensionLabel;
    QComboBox*      _xDimensionComboBox;
    QLabel*         _yDimensionLabel;
    QComboBox*      _yDimensionComboBox;
};