#pragma once

#include <memory>

#include <QWidget>

class ScatterplotPlugin;
class PixelSelectionTool;

namespace Ui {
	class AdvancedSelectionSettingsWidget;
}

/**
 * Selection tool widget class
 *
 * Provides the user interface for the pixel selection tool
 *
 * @author Thomas Kroes
 */
class AdvancedSelectionSettingsWidget : public QWidget
{
public: // Construction/destruction

    AdvancedSelectionSettingsWidget(QWidget* parent = nullptr);

    void initialize(const ScatterplotPlugin& plugin);

private:
    std::unique_ptr<Ui::AdvancedSelectionSettingsWidget>    _ui;        /** User interface as produced by Qt designer */
};