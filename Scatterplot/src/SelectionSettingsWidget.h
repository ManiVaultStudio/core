#pragma once

#include <memory>

#include <QWidget>

class ScatterplotPlugin;
class PixelSelectionTool;

namespace Ui {
	class SelectionSettingsWidget;
}

/**
 * Selection tool widget class
 *
 * Provides the user interface for the pixel selection tool
 *
 * @author Thomas Kroes
 */
class SelectionSettingsWidget : public QWidget
{
public: // Construction/destruction

    SelectionSettingsWidget(QWidget* parent = nullptr);

    void initialize(const ScatterplotPlugin& plugin);

private:
    std::unique_ptr<Ui::SelectionSettingsWidget>    _ui;        /** User interface as produced by Qt designer */
};