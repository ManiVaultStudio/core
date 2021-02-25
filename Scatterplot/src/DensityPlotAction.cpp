#include "DensityPlotAction.h"
#include "Application.h"

#include "ScatterplotWidget.h"

using namespace hdps::gui;

DensityPlotAction::DensityPlotAction(ScatterplotPlugin* scatterplotPlugin) :
    PluginAction(scatterplotPlugin, "Density"),
    _sigmaAction(this, "Sigma", 1.0, 50.0, DEFAULT_SIGMA),
    _resetAction(this, "Reset")
{
    setToolTip("Density plot settings");

    _sigmaAction.setUpdateDuringDrag(false);

    _resetAction.setToolTip("Reset plot settings");

    const auto updateRenderMode = [this]() -> void {
        setVisible(getScatterplotWidget()->getRenderMode() != ScatterplotWidget::SCATTERPLOT);
    };

    const auto updateSigma = [this]() -> void {
        getScatterplotWidget()->setSigma(0.01 * _sigmaAction.getValue());
    };

    const auto updateResetAction = [this]() -> void {
        _resetAction.setEnabled(canReset());
    };

    connect(getScatterplotWidget(), &ScatterplotWidget::renderModeChanged, this, [this, updateRenderMode, updateResetAction](const ScatterplotWidget::RenderMode& renderMode) {
        updateRenderMode();
        updateResetAction();
    });

    connect(&_sigmaAction, &DoubleAction::valueChanged, this, [this, updateSigma, updateResetAction](const double& value) {
        updateSigma();
        updateResetAction();
    });

    connect(&_resetAction, &QAction::triggered, this, [this]() {
        reset();
    });

    updateRenderMode();
    updateSigma();
    updateResetAction();
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

    menu->addSeparator();

    menu->addAction(&_resetAction);

    return menu;
}

bool DensityPlotAction::canReset() const
{
    return _sigmaAction.getValue() != DEFAULT_SIGMA;
}

void DensityPlotAction::reset()
{
    _sigmaAction.setValue(DEFAULT_SIGMA);
}

DensityPlotAction::Widget::Widget(QWidget* parent, DensityPlotAction* densityPlotAction) :
    WidgetAction::Widget(parent, densityPlotAction),
    _layout(),
    _sigmaLabel("Sigma:")
{
    _layout.addWidget(&_sigmaLabel);
    _layout.addWidget(new DoubleAction::Widget(this, &densityPlotAction->_sigmaAction));

    setLayout(&_layout);
}

DensityPlotAction::PopupWidget::PopupWidget(QWidget* parent, DensityPlotAction* densityPlotAction) :
    WidgetAction::PopupWidget(parent, densityPlotAction)
{
    auto layout = new QGridLayout();

    layout->addWidget(new QLabel("Sigma:"), 0, 0);
    layout->addWidget(new DoubleAction::Widget(this, &densityPlotAction->_sigmaAction), 0, 1);

    layout->addWidget(new StandardAction::PushButton(this, &densityPlotAction->_resetAction), 1, 1, 1, 2);

    setLayout(layout);
}