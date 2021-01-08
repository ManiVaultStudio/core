#pragma once

#include <memory>

#include <QWidget>

class ScatterplotPlugin;
class PixelSelectionTool;

namespace Ui {
	class SelectionSettingsPopupWidget;
}

/**
 * Selection tool widget class
 *
 * @author Thomas Kroes
 */
class SelectionSettingsPopupWidget : public QWidget
{
public: // Construction/destruction

    SelectionSettingsPopupWidget(QWidget* parent = nullptr);

    void initialize(const ScatterplotPlugin& plugin);

private:
    std::unique_ptr<Ui::SelectionSettingsPopupWidget>    _ui;        /** User interface as produced by Qt designer */
};