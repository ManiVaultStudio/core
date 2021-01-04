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
    auto scatterPlotWidget = const_cast<ScatterplotPlugin&>(plugin).getScatterplotWidget();

    const auto updateCurrentPage = [this, scatterPlotWidget]() {
        const auto renderMode = scatterPlotWidget->getRenderMode();

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
    };

    QObject::connect(scatterPlotWidget, &ScatterplotWidget::renderModeChanged, [this, updateCurrentPage](const ScatterplotWidget::RenderMode& renderMode) {
        updateCurrentPage();
    });

    _ui->pointSettingsWidget->initialize(plugin);
    _ui->densitySettingsWidget->initialize(plugin);

    updateCurrentPage();
}