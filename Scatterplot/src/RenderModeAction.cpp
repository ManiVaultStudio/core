#include "RenderModeAction.h"
#include "Application.h"

#include "ScatterplotPlugin.h"

#include "widgets/ActionPushButton.h"

using namespace hdps::gui;

RenderModeAction::RenderModeAction(ScatterplotPlugin* scatterplotPlugin) :
    PluginAction(scatterplotPlugin),
    _scatterPlotAction(this, "Scatter"),
    _densityPlotAction(this, "Density"),
    _contourPlotAction(this, "Contour"),
    _actionGroup(this)
{
    _scatterplotPlugin->addAction(&_scatterPlotAction);
    _scatterplotPlugin->addAction(&_densityPlotAction);
    _scatterplotPlugin->addAction(&_contourPlotAction);

    _actionGroup.addAction(&_scatterPlotAction);
    _actionGroup.addAction(&_densityPlotAction);
    _actionGroup.addAction(&_contourPlotAction);

    _scatterPlotAction.setCheckable(true);
    _densityPlotAction.setCheckable(true);
    _contourPlotAction.setCheckable(true);

    _scatterPlotAction.setShortcut(QKeySequence("S"));
    _densityPlotAction.setShortcut(QKeySequence("D"));
    _contourPlotAction.setShortcut(QKeySequence("C"));

    _scatterPlotAction.setToolTip("Set render mode to scatter plot");
    _densityPlotAction.setToolTip("Set render mode to density plot");
    _contourPlotAction.setToolTip("Set render mode to contour plot");

    /*
    const auto& fontAwesome = Application::getIconFont("FontAwesome");

    _scatterPlotAction.setIcon(fontAwesome.getIcon("braille"));
    _densityPlotAction.setIcon(fontAwesome.getIcon("cloud"));
    _contourPlotAction.setIcon(fontAwesome.getIcon("mountain"));
    */

    connect(&_scatterPlotAction, &QAction::toggled, this, [this](bool toggled) {
        if (toggled)
            getScatterplotWidget()->setRenderMode(ScatterplotWidget::RenderMode::SCATTERPLOT);
    });

    connect(&_densityPlotAction, &QAction::toggled, this, [this](bool toggled) {
        if (toggled)
            getScatterplotWidget()->setRenderMode(ScatterplotWidget::RenderMode::DENSITY);
    });

    connect(&_contourPlotAction, &QAction::toggled, this, [this](bool toggled) {
        if (toggled)
            getScatterplotWidget()->setRenderMode(ScatterplotWidget::RenderMode::LANDSCAPE);
    });

    const auto updateButtons = [this]() -> void {
        const auto renderMode = getScatterplotWidget()->getRenderMode();

        _scatterPlotAction.setChecked(renderMode == ScatterplotWidget::RenderMode::SCATTERPLOT);
        _densityPlotAction.setChecked(renderMode == ScatterplotWidget::RenderMode::DENSITY);
        _contourPlotAction.setChecked(renderMode == ScatterplotWidget::RenderMode::LANDSCAPE);
    };

    connect(getScatterplotWidget(), &ScatterplotWidget::renderModeChanged, this, [this, updateButtons](const ScatterplotWidget::RenderMode& renderMode) {
        updateButtons();
    });

    updateButtons();
}

QMenu* RenderModeAction::getContextMenu()
{
    auto menu = new QMenu("Render mode");

    menu->addAction(&_scatterPlotAction);
    menu->addAction(&_densityPlotAction);
    menu->addAction(&_contourPlotAction);

    return menu;
}

RenderModeAction::Widget::Widget(QWidget* parent, RenderModeAction* renderModeAction) :
    WidgetAction::Widget(parent, renderModeAction),
    _layout()
{
    _layout.addWidget(renderModeAction->_scatterPlotAction.createWidget(this));
    _layout.addWidget(renderModeAction->_densityPlotAction.createWidget(this));
    _layout.addWidget(renderModeAction->_contourPlotAction.createWidget(this));

    setLayout(&_layout);
}