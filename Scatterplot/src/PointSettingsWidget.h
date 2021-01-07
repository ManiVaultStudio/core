#pragma once

#include <QWidget>

class ScatterplotPlugin;

namespace Ui {
    class PointSettingsWidget;
}

class PointSettingsWidget : public QWidget
{
public:
    PointSettingsWidget(QWidget* parent = nullptr);

    void initialize(const ScatterplotPlugin& plugin);

private:
    std::unique_ptr<Ui::PointSettingsWidget>        _ui;        /** Externally loaded UI */
};