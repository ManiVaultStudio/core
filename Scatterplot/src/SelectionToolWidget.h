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
 * Provides the user interface for the pixel selection tool
 *
 * @author Thomas Kroes
 */
class SelectionToolWidget : public QWidget
{
public: // Construction/destruction

    /**
     * Constructor
     * @param scatterplotPlugin Pointer to the scatter plot plugin instance
     */
    SelectionToolWidget(ScatterplotPlugin* scatterplotPlugin);

private:

    /** Convenience function to get the pixel selection tool from the scatter plot plugin */
    PixelSelectionTool& getPixelSelectionTool();

private:
    ScatterplotPlugin*                          _scatterplotPlugin;     /** Scatter plot plugin instance */
    std::unique_ptr<Ui::SelectionToolWidget>    _ui;                    /** User interface as produced by Qt designer */
};