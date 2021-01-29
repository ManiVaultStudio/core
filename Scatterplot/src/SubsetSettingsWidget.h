#pragma once

#include "ScatterplotSettingsWidget.h"

class QPushButton;
class QCheckBox;

class SubsetSettingsWidget : public ScatterplotSettingsWidget
{
public:
    SubsetSettingsWidget(const hdps::gui::ResponsiveToolBar::WidgetState& state, QWidget* parent = nullptr);

protected:
    void connectToPlugin() override;

private:
    QPushButton*    _createSubsetPushButton;
    QCheckBox*      _fromSourceCheckBox;
};