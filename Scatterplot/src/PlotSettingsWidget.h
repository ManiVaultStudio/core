#pragma once

#include <QWidget>

class ScatterplotPlugin;

namespace Ui {
    class PlotSettingsWidget;
}

class PlotSettingsWidget : public QWidget
{
public:
    PlotSettingsWidget(const ScatterplotPlugin& plugin);

private:
    std::unique_ptr<Ui::PlotSettingsWidget>        _ui;        /** Externally loaded UI */
};