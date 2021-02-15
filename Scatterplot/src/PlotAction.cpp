#include "PlotAction.h"
#include "ScatterplotWidget.h"

using namespace hdps::gui;

PlotAction::PlotAction(ScatterplotPlugin* scatterplotPlugin) :
    PluginAction(scatterplotPlugin),
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
    _layout()
{
    _layout.addWidget(new PointPlotAction::Widget(this, &plotAction->_pointPlotAction));
    _layout.addWidget(new DensityPlotAction::Widget(this, &plotAction->_densityPlotAction));

    setLayout(&_layout);
}