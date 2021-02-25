#include "PlotAction.h"
#include "ScatterplotWidget.h"

using namespace hdps::gui;

PlotAction::PlotAction(ScatterplotPlugin* scatterplotPlugin) :
    PluginAction(scatterplotPlugin, "Plot"),
    _pointPlotAction(scatterplotPlugin),
    _densityPlotAction(scatterplotPlugin)
{
}

QMenu* PlotAction::getContextMenu()
{
    switch (getScatterplotWidget()->getRenderMode())
    {
        case ScatterplotWidget::RenderMode::SCATTERPLOT:
            return _pointPlotAction.getContextMenu();
            break;

        case ScatterplotWidget::RenderMode::DENSITY:
        case ScatterplotWidget::RenderMode::LANDSCAPE:
            return _densityPlotAction.getContextMenu();
            break;

        default:
            break;
    }

    return new QMenu("Plot");
}

PlotAction::Widget::Widget(QWidget* parent, PlotAction* plotAction) :
    WidgetAction::Widget(parent, plotAction),
    _layout(),
    _pointPlotWidget(this, &plotAction->_pointPlotAction),
    _densityPlotWidget(this, &plotAction->_densityPlotAction)
{
    _layout.addWidget(&_pointPlotWidget);
    _layout.addWidget(&_densityPlotWidget);

    const auto updateRenderMode = [this, plotAction]() -> void {
        const auto renderMode = plotAction->getScatterplotWidget()->getRenderMode();

        _pointPlotWidget.setVisible(renderMode == ScatterplotWidget::RenderMode::SCATTERPLOT);
        _densityPlotWidget.setVisible(renderMode != ScatterplotWidget::RenderMode::SCATTERPLOT);
    };

    connect(plotAction->getScatterplotWidget(), &ScatterplotWidget::renderModeChanged, this, [this, updateRenderMode](const ScatterplotWidget::RenderMode& renderMode) {
        updateRenderMode();
    });

    updateRenderMode();

    setLayout(&_layout);
}

PlotAction::PopupWidget::PopupWidget(QWidget* parent, PlotAction* plotAction) :
    WidgetAction::PopupWidget(parent, plotAction),
    _pointPlotWidget(this, &plotAction->_pointPlotAction),
    _densityPlotWidget(this, &plotAction->_densityPlotAction)
{
    auto layout = new QVBoxLayout();

    layout->addWidget(&_pointPlotWidget);
    layout->addWidget(&_densityPlotWidget);

    const auto updateRenderMode = [this, plotAction]() -> void {
        const auto renderMode = plotAction->getScatterplotWidget()->getRenderMode();

        _pointPlotWidget.setVisible(renderMode == ScatterplotWidget::RenderMode::SCATTERPLOT);
        _densityPlotWidget.setVisible(renderMode != ScatterplotWidget::RenderMode::SCATTERPLOT);
    };

    connect(plotAction->getScatterplotWidget(), &ScatterplotWidget::renderModeChanged, this, [this, updateRenderMode](const ScatterplotWidget::RenderMode& renderMode) {
        updateRenderMode();
    });

    updateRenderMode();

    setLayout(layout);
}