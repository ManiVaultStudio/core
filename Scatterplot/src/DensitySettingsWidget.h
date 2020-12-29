#pragma once

#include <QWidget>

class ScatterplotPlugin;

namespace Ui {
    class DensitySettingsWidget;
}

class DensitySettingsWidget : public QWidget
{
public:
    DensitySettingsWidget(const ScatterplotPlugin& plugin);

private:
    std::unique_ptr<Ui::DensitySettingsWidget>        _ui;        /** Externally loaded UI */
};