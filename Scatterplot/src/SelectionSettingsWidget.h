#pragma once

#include <memory>

#include <QWidget>

class ScatterplotPlugin;
class PixelSelectionTool;

namespace Ui {
	class SelectionSettingsWidget;
}

class SelectionSettingsWidget : public QWidget
{
public:

    SelectionSettingsWidget(QWidget* parent = nullptr);

    void initialize(const ScatterplotPlugin& plugin);

private:
    std::unique_ptr<Ui::SelectionSettingsWidget>    _ui;        /** User interface as produced by Qt designer */
};