#include "PlotAction.h"
#include "Application.h"

#include "ScatterplotWidget.h"

using namespace hdps::gui;

PlotAction::PlotAction(ScatterplotPlugin* scatterplotPlugin) :
    PluginAction(scatterplotPlugin),
    _pointSizeAction(this, "Point size", 0.0, 50.0, DEFAULT_POINT_SIZE),
    _pointOpacityAction(this, "Point opacity", 0.0, 100.0, DEFAULT_POINT_OPACITY),
    _sigmaAction(this, "Sigma", 0.0, 50.0, DEFAULT_SIGMA),
    _resetAction("Reset")
{
    _pointSizeAction.setSuffix("px");
    _pointOpacityAction.setSuffix("%");
    _sigmaAction.setUpdateDuringDrag(false);

    _resetAction.setToolTip("Reset plot settings");

    const auto updateRenderMode = [this]() -> void {
        const auto renderMode = getScatterplotWidget()->getRenderMode();

        _pointSizeAction.setVisible(renderMode == ScatterplotWidget::SCATTERPLOT);
        _pointOpacityAction.setVisible(renderMode == ScatterplotWidget::SCATTERPLOT);
        _sigmaAction.setVisible(renderMode != ScatterplotWidget::SCATTERPLOT);
    };

    const auto updatePointSize = [this]() -> void {
        getScatterplotWidget()->setPointSize(_pointSizeAction.getValue());
    };

    const auto updatePointOpacity = [this]() -> void {
        getScatterplotWidget()->setAlpha(0.01 * _pointOpacityAction.getValue());
    };

    const auto updateSigma = [this]() -> void {
        getScatterplotWidget()->setSigma(0.01 * _sigmaAction.getValue());
    };

    const auto updateResetAction = [this]() -> void {
        const auto renderMode = getScatterplotWidget()->getRenderMode();

        auto enabled = false;

        switch (renderMode)
        {
            case ScatterplotWidget::SCATTERPLOT:
                enabled = !(_pointSizeAction.getValue() == DEFAULT_POINT_SIZE && _pointOpacityAction.getValue() == DEFAULT_POINT_OPACITY);
                break;

            case ScatterplotWidget::DENSITY:
            case ScatterplotWidget::LANDSCAPE:
                enabled = _sigmaAction.getValue() != DEFAULT_SIGMA;
                break;

            default:
                break;
        }

        _resetAction.setEnabled(enabled);
    };

    connect(getScatterplotWidget(), &ScatterplotWidget::renderModeChanged, this, [this, updateRenderMode, updateResetAction](const ScatterplotWidget::RenderMode& renderMode) {
        updateRenderMode();
        updateResetAction();
    });

    connect(&_pointSizeAction, &DoubleAction::valueChanged, this, [this, updatePointSize, updateResetAction](const double& value) {
        updatePointSize();
        updateResetAction();
    });

    connect(&_pointOpacityAction, &DoubleAction::valueChanged, this, [this, updatePointOpacity, updateResetAction](const double& value) {
        updatePointOpacity();
        updateResetAction();
    });

    connect(&_sigmaAction, &DoubleAction::valueChanged, this, [this, updateSigma, updateResetAction](const double& value) {
        updateSigma();
        updateResetAction();
    });

    connect(&_resetAction, &QAction::triggered, this, [this]() {
        const auto renderMode = getScatterplotWidget()->getRenderMode();

        switch (renderMode)
        {
            case ScatterplotWidget::SCATTERPLOT:
                _pointSizeAction.setValue(DEFAULT_POINT_SIZE);
                _pointOpacityAction.setValue(DEFAULT_POINT_OPACITY);
                break;

            case ScatterplotWidget::DENSITY:
            case ScatterplotWidget::LANDSCAPE:
                _sigmaAction.setValue(DEFAULT_SIGMA);
                break;

            default:
                break;
        }
    });

    updateRenderMode();
    updatePointSize();
    updatePointOpacity();
    updateSigma();
    updateResetAction();
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
            addActionToMenu(&_pointSizeAction);
            addActionToMenu(&_pointOpacityAction);
            break;

        case ScatterplotWidget::RenderMode::DENSITY:
        case ScatterplotWidget::RenderMode::LANDSCAPE:
            addActionToMenu(&_sigmaAction);
            break;

        default:
            break;
    }

    menu->addSeparator();
    menu->addAction(&_resetAction);

    return menu;
}

PlotAction::Widget::Widget(QWidget* parent, PlotAction* plotAction) :
    WidgetAction::Widget(parent, plotAction),
    _layout()
{
    _layout.addWidget(plotAction->_pointSizeAction.createWidget(this));
    _layout.addWidget(plotAction->_pointOpacityAction.createWidget(this));
    _layout.addWidget(plotAction->_sigmaAction.createWidget(this));

    setLayout(&_layout);
}