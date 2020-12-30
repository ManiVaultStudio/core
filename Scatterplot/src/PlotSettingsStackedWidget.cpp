#include "PlotSettingsStackedWidget.h"
#include "ScatterplotPlugin.h"

#include "ui_PlotSettingsStackedWidget.h"

PlotSettingsStackedWidget::PlotSettingsStackedWidget(QWidget* parent /*= nullptr*/) :
    QStackedWidget(parent),
    _ui{ std::make_unique<Ui::PlotSettingsStackedWidget>() }
{
    _ui->setupUi(this);
}

void PlotSettingsStackedWidget::initialize(const ScatterplotPlugin& plugin)
{
    QObject::connect(plugin._scatterPlotWidget, &ScatterplotWidget::renderModeChanged, [this](const ScatterplotWidget::RenderMode& renderMode) {
        switch (renderMode)
        {
            case ScatterplotWidget::RenderMode::SCATTERPLOT:
                setCurrentIndex(0);
                break;

            case ScatterplotWidget::RenderMode::DENSITY:
            case ScatterplotWidget::RenderMode::LANDSCAPE:
                setCurrentIndex(1);
                break;
        }
    });

    _ui->pointSettingsWidget->initialize(plugin);
    _ui->densitySettingsWidget->initialize(plugin);
}