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
    auto menu = new QMenu("Plot");

    const auto renderMode = getScatterplotWidget()->getRenderMode();

    const auto addActionToMenu = [menu](QAction* action) {
        auto actionMenu = new QMenu(action->text());

        actionMenu->addAction(action);

        menu->addMenu(actionMenu);
    };

    switch (renderMode)
    {
        case ScatterplotWidget::RenderMode::SCATTERPLOT:
            addActionToMenu(&_pointPlotAction);
            break;

        case ScatterplotWidget::RenderMode::DENSITY:
        case ScatterplotWidget::RenderMode::LANDSCAPE:
            addActionToMenu(&_densityPlotAction);
            break;

        default:
            break;
    }

    return menu;
}

PlotAction::Widget::Widget(QWidget* parent, PlotAction* plotAction) :
    WidgetAction::Widget(parent, plotAction),
    _layout()
{
    _layout.addWidget(plotAction->_pointPlotAction.createWidget(this));
    _layout.addWidget(plotAction->_densityPlotAction.createWidget(this));

    setLayout(&_layout);
}