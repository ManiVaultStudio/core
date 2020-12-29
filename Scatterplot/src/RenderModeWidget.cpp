#include "RenderModeWidget.h"
#include "ScatterplotPlugin.h"

#include "ui_RenderModeWidget.h"

RenderModeWidget::RenderModeWidget(const ScatterplotPlugin& plugin) :
    QWidget(),
    _ui{ std::make_unique<Ui::RenderModeWidget>() }
{
    _ui->setupUi(this);

    const auto updateToggles = [this, &plugin]() {
        const auto renderMode = plugin._scatterPlotWidget->getRenderMode();

        _ui->scatterPlotPushButton->setChecked(renderMode == ScatterplotWidget::RenderMode::SCATTERPLOT);
        _ui->densityPlotPushButton->setChecked(renderMode == ScatterplotWidget::RenderMode::DENSITY);
        _ui->contourPlotPushButton->setChecked(renderMode == ScatterplotWidget::RenderMode::LANDSCAPE);
    };

    QObject::connect(_ui->scatterPlotPushButton, &QPushButton::clicked, [this, &plugin, updateToggles]() {
        plugin._scatterPlotWidget->setRenderMode(ScatterplotWidget::RenderMode::SCATTERPLOT);
        updateToggles();
    });

    QObject::connect(_ui->densityPlotPushButton, &QPushButton::clicked, [this, &plugin, updateToggles]() {
        plugin._scatterPlotWidget->setRenderMode(ScatterplotWidget::RenderMode::DENSITY);
        updateToggles();
    });

    QObject::connect(_ui->contourPlotPushButton, &QPushButton::clicked, [this, &plugin, updateToggles]() {
        plugin._scatterPlotWidget->setRenderMode(ScatterplotWidget::RenderMode::LANDSCAPE);
        updateToggles();
    });

    QObject::connect(plugin._scatterPlotWidget, &ScatterplotWidget::renderModeChanged, [this, updateToggles](const ScatterplotWidget::RenderMode& renderMode) {
        updateToggles();
    });

    updateToggles();
}