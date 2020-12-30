#include "PlotSettingsWidget.h"
#include "ScatterplotPlugin.h"

#include "ui_PlotSettingsWidget.h"

PlotSettingsWidget::PlotSettingsWidget(const ScatterplotPlugin& plugin) :
    QWidget(),
    _ui{ std::make_unique<Ui::PlotSettingsWidget>() }
{
    _ui->setupUi(this);

    QObject::connect(plugin._scatterPlotWidget, &ScatterplotWidget::renderModeChanged, [this](const ScatterplotWidget::RenderMode& renderMode) {
        switch (renderMode)
        {
            case ScatterplotWidget::RenderMode::SCATTERPLOT:
                _ui->stackedWidget->setCurrentIndex(0);
                break;

            case ScatterplotWidget::RenderMode::DENSITY:
            case ScatterplotWidget::RenderMode::LANDSCAPE:
                _ui->stackedWidget->setCurrentIndex(1);
                break;
        }
    });

    _ui->pointSettingsWidget->initialize(plugin);
    _ui->densitySettingsWidget->initialize(plugin);
}