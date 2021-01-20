#pragma once

#include "widgets/ResponsiveToolBar.h"

class ScatterplotPlugin;

class QLabel;
class QComboBox;

class PositionSettingsWidget : public hdps::gui::ResponsiveToolBar::Widget
{
public:
    PositionSettingsWidget(QWidget* parent = nullptr);

    void initialize(const ScatterplotPlugin& plugin);

protected:

    void updateState() override;

public:

    int getDimensionX();
    int getDimensionY();

    void setDimensions(unsigned int numDimensions, const std::vector<QString>& names = std::vector<QString>());

private:
    QLabel*     _xDimensionLabel;
    QComboBox*  _xDimensionComboBox;
    QLabel*     _yDimensionLabel;
    QComboBox*  _yDimensionComboBox;
};