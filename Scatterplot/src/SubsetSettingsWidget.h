#pragma once

#include "widgets/ResponsiveToolBar.h"

class ScatterplotPlugin;

class QPushButton;
class QCheckBox;

class SubsetSettingsWidget : public hdps::gui::ResponsiveToolBar::StatefulWidget
{
public:
    SubsetSettingsWidget(QWidget* parent = nullptr);

    void initializeUI();
    void setScatterPlotPlugin(const ScatterplotPlugin& plugin);

protected:
    QLayout* getLayout(const hdps::gui::ResponsiveToolBar::WidgetState& state) override;

private:
    QPushButton*    _createSubsetPushButton;
    QCheckBox*      _fromSourceCheckBox;
};