#pragma once

#include <QWidget>

class ScatterplotPlugin;

namespace Ui {
    class RenderModeWidget;
}

class RenderModeWidget : public QWidget
{
public:
    RenderModeWidget(const ScatterplotPlugin& plugin);

private:
    std::unique_ptr<Ui::RenderModeWidget>        _ui;        /** Externally loaded UI */
};