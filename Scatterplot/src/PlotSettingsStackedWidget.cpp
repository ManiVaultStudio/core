#include "PlotSettingsStackedWidget.h"
#include "ScatterplotPlugin.h"
#include "PointSettingsWidget.h"
#include "DensitySettingsWidget.h"

PlotSettingsStackedWidget::PlotSettingsStackedWidget(QWidget* parent /*= nullptr*/) :
    QStackedWidget(parent),
    _pointSettingsWidget(new StateWidget<PointSettingsWidget>(this)),
    _densitySettingsWidget(new StateWidget<DensitySettingsWidget>(this))
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

    _pointSettingsWidget->setListenWidget(&const_cast<ScatterplotPlugin&>(plugin));
    _pointSettingsWidget->getWidget()->initialize(plugin);

    _densitySettingsWidget->setListenWidget(&const_cast<ScatterplotPlugin&>(plugin));
    _densitySettingsWidget->getWidget()->initialize(plugin);

    updateCurrentPage();
}

PointSettingsWidget* PlotSettingsStackedWidget::getPointSettingsWidget()
{
    return _pointSettingsWidget->getWidget();
}

DensitySettingsWidget* PlotSettingsStackedWidget::getDensitySettingsWidget()
{
    return _densitySettingsWidget->getWidget();
}