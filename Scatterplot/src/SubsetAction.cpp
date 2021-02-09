#include "SubsetAction.h"

#include "ScatterplotPlugin.h"

#include "widgets/ActionPushButton.h"

using namespace hdps::gui;

SubsetAction::SubsetAction(ScatterplotPlugin* scatterplotPlugin) :
    PluginAction(scatterplotPlugin),
    _createSubsetAction("Create subset"),
    _fromSourceDataAction("From source data")
{
    _fromSourceDataAction.setCheckable(true);

    /*
    _createSubsetAction.setShortcut(QKeySequence("D"));
    _fromSourceDataAction.setShortcut(QKeySequence("C"));
    */

    _createSubsetAction.setToolTip("Create subset from selected data points");
    _fromSourceDataAction.setToolTip("Create subset from source data");

    /*
    const auto& fontAwesome = Application::getIconFont("FontAwesome");

    _scatterPlotAction.setIcon(fontAwesome.getIcon("braille"));
    _densityPlotAction.setIcon(fontAwesome.getIcon("cloud"));
    _contourPlotAction.setIcon(fontAwesome.getIcon("mountain"));

    connect(&_scatterPlotAction, &QAction::triggered, this, [this, scatterplotWidget]() {
        scatterplotWidget->setRenderMode(ScatterplotWidget::RenderMode::SCATTERPLOT);
    });

    connect(&_densityPlotAction, &QAction::triggered, this, [this, scatterplotWidget]() {
        scatterplotWidget->setRenderMode(ScatterplotWidget::RenderMode::DENSITY);
    });

    connect(&_contourPlotAction, &QAction::triggered, this, [this, scatterplotWidget]() {
        scatterplotWidget->setRenderMode(ScatterplotWidget::RenderMode::LANDSCAPE);
    });

    const auto updateButtons = [this, scatterplotWidget]() {
        const auto renderMode = scatterplotWidget->getRenderMode();

        QSignalBlocker scatterPlotActionBlocker(&_scatterPlotAction), densityPlotAction(&_densityPlotAction), contourPlotAction(&_contourPlotAction);

        _scatterPlotAction.setChecked(renderMode == ScatterplotWidget::RenderMode::SCATTERPLOT);
        _densityPlotAction.setChecked(renderMode == ScatterplotWidget::RenderMode::DENSITY);
        _contourPlotAction.setChecked(renderMode == ScatterplotWidget::RenderMode::LANDSCAPE);
    };

    connect(scatterplotWidget, &ScatterplotWidget::renderModeChanged, this, [this, updateButtons](const ScatterplotWidget::RenderMode& renderMode) {
        updateButtons();
    });

    updateButtons();
    */
}

QMenu* SubsetAction::getContextMenu()
{
    auto menu = new QMenu("Render mode");

    menu->addAction(&_createSubsetAction);
    menu->addAction(&_fromSourceDataAction);

    return menu;
}

SubsetAction::Widget::Widget(QWidget* parent, SubsetAction* subsetAction) :
    WidgetAction::Widget(parent, subsetAction),
    _layout(),
    _toolButton(),
    _popupWidget(this, "Subset"),
    _popupWidgetAction(this)
{
    _layout.addWidget(&_toolButton);
    
    _popupWidgetAction.setDefaultWidget(&_popupWidget);

    auto popupWidgetLayout = new QVBoxLayout();

    popupWidgetLayout->addWidget(new ActionPushButton(&subsetAction->_createSubsetAction));

    _popupWidget.setContentLayout(popupWidgetLayout);

    _toolButton.setPopupMode(QToolButton::InstantPopup);
    _toolButton.addAction(&_popupWidgetAction);
    

    setLayout(&_layout);
}