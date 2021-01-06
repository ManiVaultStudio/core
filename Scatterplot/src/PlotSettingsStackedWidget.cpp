#include "PlotSettingsStackedWidget.h"
#include "ScatterplotPlugin.h"
#include "PointSettingsWidget.h"
#include "DensitySettingsWidget.h"

PlotSettingsStackedWidget::PlotSettingsStackedWidget(QWidget* parent /*= nullptr*/) :
    QStackedWidget(parent),
    _pointSettingsWidget(new PointSettingsWidget(this)),
    _densitySettingsWidget(new DensitySettingsWidget(this))
{
    addWidget(_pointSettingsWidget);
    addWidget(_densitySettingsWidget);
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

    _pointSettingsWidget->initialize(plugin);
    _densitySettingsWidget->initialize(plugin);

    updateCurrentPage();
}

PointSettingsWidget* PlotSettingsStackedWidget::getPointSettingsWidget()
{
    return _pointSettingsWidget;
}

DensitySettingsWidget* PlotSettingsStackedWidget::getDensitySettingsWidget()
{
    return _densitySettingsWidget;
}