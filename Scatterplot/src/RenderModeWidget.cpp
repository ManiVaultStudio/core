#include "RenderModeWidget.h"
#include "ScatterplotPlugin.h"

#include "ui_RenderModeWidget.h"

RenderModeWidget::RenderModeWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _ui{ std::make_unique<Ui::RenderModeWidget>() }
{
    _ui->setupUi(this);
}

void RenderModeWidget::initialize(const ScatterplotPlugin& plugin)
{
    auto scatterPlotWidget = const_cast<ScatterplotPlugin&>(plugin).getScatterplotWidget();

    const auto updateToggles = [this, scatterPlotWidget]() {
        const auto renderMode = scatterPlotWidget->getRenderMode();

        _ui->scatterPlotPushButton->setChecked(renderMode == ScatterplotWidget::RenderMode::SCATTERPLOT);
        _ui->densityPlotPushButton->setChecked(renderMode == ScatterplotWidget::RenderMode::DENSITY);
        _ui->contourPlotPushButton->setChecked(renderMode == ScatterplotWidget::RenderMode::LANDSCAPE);
    };

    QObject::connect(_ui->scatterPlotPushButton, &QPushButton::clicked, [this, scatterPlotWidget, updateToggles]() {
        scatterPlotWidget->setRenderMode(ScatterplotWidget::RenderMode::SCATTERPLOT);
        updateToggles();
    });

    QObject::connect(_ui->densityPlotPushButton, &QPushButton::clicked, [this, scatterPlotWidget, updateToggles]() {
        scatterPlotWidget->setRenderMode(ScatterplotWidget::RenderMode::DENSITY);
        updateToggles();
    });

    QObject::connect(_ui->contourPlotPushButton, &QPushButton::clicked, [this, scatterPlotWidget, updateToggles]() {
        scatterPlotWidget->setRenderMode(ScatterplotWidget::RenderMode::LANDSCAPE);
        updateToggles();
    });

    QObject::connect(scatterPlotWidget, &ScatterplotWidget::renderModeChanged, [this, updateToggles](const ScatterplotWidget::RenderMode& renderMode) {
        updateToggles();
    });

    updateToggles();
}