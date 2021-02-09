#include "RenderModeAction.h"
#include "Application.h"

#include "ScatterplotPlugin.h"

using namespace hdps::gui;

RenderModeAction::RenderModeAction(ScatterplotPlugin* scatterplotPlugin) :
    PluginAction(scatterplotPlugin),
    _scatterPlotAction("Scatter plot"),
    _densityPlotAction("Density plot"),
    _contourPlotAction("Contour plot"),
    _actionGroup(this)
{
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

    const auto& fontAwesome = Application::getIconFont("FontAwesome");

    _scatterPlotAction.setIcon(fontAwesome.getIcon("braille"));
    _densityPlotAction.setIcon(fontAwesome.getIcon("cloud"));
    _contourPlotAction.setIcon(fontAwesome.getIcon("mountain"));

    connect(&_scatterPlotAction, &QAction::triggered, this, [this]() {
        getScatterplotWidget()->setRenderMode(ScatterplotWidget::RenderMode::SCATTERPLOT);
    });

    connect(&_densityPlotAction, &QAction::triggered, this, [this]() {
        getScatterplotWidget()->setRenderMode(ScatterplotWidget::RenderMode::DENSITY);
    });

    connect(&_contourPlotAction, &QAction::triggered, this, [this]() {
        getScatterplotWidget()->setRenderMode(ScatterplotWidget::RenderMode::LANDSCAPE);
    });

    const auto updateButtons = [this]() {
        const auto renderMode = getScatterplotWidget()->getRenderMode();

        QSignalBlocker scatterPlotActionBlocker(&_scatterPlotAction), densityPlotAction(&_densityPlotAction), contourPlotAction(&_contourPlotAction);

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
    _layout(),
    _toolBar(),
    _toolButton(),
    _popupWidget(this, "Render mode"),
    _popupWidgetAction(this)
{
    _layout.addWidget(&_toolBar);

    _toolBar.addAction(&renderModeAction->_scatterPlotAction);
    _toolBar.addAction(&renderModeAction->_densityPlotAction);
    _toolBar.addAction(&renderModeAction->_contourPlotAction);

    _popupWidgetAction.setDefaultWidget(&_popupWidget);

    _toolButton.setPopupMode(QToolButton::InstantPopup);
    _toolButton.addAction(&_popupWidgetAction);

    setLayout(&_layout);
}