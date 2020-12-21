#pragma once

#include <memory>

#include <QWidget>

class ScatterplotPlugin;
class PixelSelectionTool;

namespace Ui {
	class SelectionToolWidget;
}

/**
 * Selection tool widget class
 *
 * @author Thomas Kroes
 */
class SelectionToolWidget : public QWidget
{
public: // Construction/destruction

    SelectionToolWidget(ScatterplotPlugin* scatterplotPlugin);

private:
    PixelSelectionTool& getSelectionTool();

private:
    ScatterplotPlugin*                          _scatterplotPlugin;     /** Scatter plot plugin instance */
    std::unique_ptr<Ui::SelectionToolWidget>    _ui;                    /** User interface as produced by Qt designer */
};