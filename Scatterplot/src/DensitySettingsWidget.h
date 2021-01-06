#pragma once

#include <QWidget>

class ScatterplotPlugin;

namespace Ui {
    class DensitySettingsWidget;
}

class DensitySettingsWidget : public QWidget
{
public:
    DensitySettingsWidget(QWidget* parent = nullptr);

    void initialize(const ScatterplotPlugin& plugin);

    bool eventFilter(QObject* target, QEvent* event) override;

private:
    std::unique_ptr<Ui::DensitySettingsWidget>        _ui;        /** Externally loaded UI */
};