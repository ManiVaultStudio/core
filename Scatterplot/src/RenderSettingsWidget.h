#pragma once

#include <QWidget>

class ScatterplotPlugin;

namespace Ui {
    class RenderSettingsWidget;
}

class RenderSettingsWidget : public QWidget
{
public:
    RenderSettingsWidget(const ScatterplotPlugin& plugin);

private:
    std::unique_ptr<Ui::RenderSettingsWidget>        _ui;        /** Externally loaded UI */
};