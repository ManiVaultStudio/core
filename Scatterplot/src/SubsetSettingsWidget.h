#pragma once

#include "ScatterplotSettingsWidget.h"

class QPushButton;
class QCheckBox;

class SubsetSettingsWidget : public ScatterplotSettingsWidget
{
public:
    SubsetSettingsWidget(const hdps::gui::ResponsiveSectionWidget::State& state, QWidget* parent = nullptr);

    void setScatterplotPlugin(ScatterplotPlugin* scatterplotPlugin) override;

private:
    QPushButton*    _createSubsetPushButton;
    QCheckBox*      _fromSourceCheckBox;
};