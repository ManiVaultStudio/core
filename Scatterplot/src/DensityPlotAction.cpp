#include "DensityPlotAction.h"
#include "Application.h"

#include "ScatterplotPlugin.h"
#include "ScatterplotWidget.h"

using namespace hdps::gui;

DensityPlotAction::DensityPlotAction(ScatterplotPlugin* scatterplotPlugin) :
    PluginAction(scatterplotPlugin, "Density"),
    _sigmaAction(this, "Sigma", 1.0, 50.0, DEFAULT_SIGMA, DEFAULT_SIGMA)
{
    setToolTip("Density plot settings");

    //

    const auto updateRenderMode = [this]() -> void {
        setVisible(getScatterplotWidget()->getRenderMode() != ScatterplotWidget::SCATTERPLOT);
    };

    const auto updateSigma = [this]() -> void {
        getScatterplotWidget()->setSigma(0.01 * _sigmaAction.getValue());
    };

    connect(getScatterplotWidget(), &ScatterplotWidget::renderModeChanged, this, [this, updateRenderMode](const ScatterplotWidget::RenderMode& renderMode) {
        updateRenderMode();
    });

    connect(&_sigmaAction, &DoubleAction::valueChanged, this, [this, updateSigma](const double& value) {
        updateSigma();
    });

    const auto updateSigmaAction = [this]() {
        _sigmaAction.setUpdateDuringDrag(_scatterplotPlugin->getNumberOfPoints() < 100000);
    };

    connect(scatterplotPlugin, &ScatterplotPlugin::currentDatasetChanged, this, [this, updateSigmaAction](const QString& datasetName) {
        updateSigmaAction();
    });

    updateSigmaAction();

    updateRenderMode();
    updateSigma();
}

QMenu* DensityPlotAction::getContextMenu()
{
    auto menu = new QMenu("Plot settings");

    const auto renderMode = getScatterplotWidget()->getRenderMode();

    const auto addActionToMenu = [menu](QAction* action) {
        auto actionMenu = new QMenu(action->text());

        actionMenu->addAction(action);

        menu->addMenu(actionMenu);
    };

    addActionToMenu(&_sigmaAction);

    return menu;
}

DensityPlotAction::Widget::Widget(QWidget* parent, DensityPlotAction* densityPlotAction) :
    WidgetAction::Widget(parent, densityPlotAction),
    _layout(),
    _sigmaLabel("Sigma:")
{
    _layout.addWidget(&_sigmaLabel);
    _layout.addWidget(new DoubleAction::Widget(this, &densityPlotAction->_sigmaAction, DoubleAction::Widget::Configuration::Slider));

    setLayout(&_layout);
}

DensityPlotAction::PopupWidget::PopupWidget(QWidget* parent, DensityPlotAction* densityPlotAction) :
    WidgetAction::PopupWidget(parent, densityPlotAction)
{
    auto layout = new QGridLayout();

    layout->addWidget(new QLabel("Sigma:"), 0, 0);
    layout->addWidget(new DoubleAction::Widget(this, &densityPlotAction->_sigmaAction), 0, 1);

    setLayout(layout);
}