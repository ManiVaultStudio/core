#pragma once

#include <QWidget>

class ScatterplotPlugin;

namespace Ui {
    class SubsetSettingsWidget;
}

class SubsetSettingsWidget : public QWidget
{
public:
    SubsetSettingsWidget(QWidget* parent = nullptr);

    void initialize(const ScatterplotPlugin& plugin);

private:
    std::unique_ptr<Ui::SubsetSettingsWidget>        _ui;        /** Externally loaded UI */
};