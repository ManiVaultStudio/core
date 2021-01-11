#include "PlotSettingsStackedWidget.h"
#include "ScatterplotPlugin.h"
#include "PointSettingsWidget.h"
#include "DensitySettingsWidget.h"

PlotSettingsStackedWidget::PlotSettingsStackedWidget(QWidget* parent /*= nullptr*/) :
    QStackedWidget(parent),
    _pointSettingsWidget(new StateWidget<PointSettingsWidget>(this)),
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

    _pointSettingsWidget->initialize(&const_cast<ScatterplotPlugin&>(plugin), [this](const QSize& sourceWidgetSize) {
        const auto width = sourceWidgetSize.width();

        auto state = WidgetStateMixin::State::Popup;

        if (width >= 800 && width < 1500)
            state = WidgetStateMixin::State::Compact;

        if (width >= 1500)
            state = WidgetStateMixin::State::Full;

        return state;
    }, [this, &plugin](PointSettingsWidget* pointSettingsWidget) {
        pointSettingsWidget->initialize(plugin);
    });

    _densitySettingsWidget->initialize(plugin);

    updateCurrentPage();
}

PointSettingsWidget* PlotSettingsStackedWidget::getPointSettingsWidget()
{
    return nullptr;
    //return _pointSettingsWidget;
}

DensitySettingsWidget* PlotSettingsStackedWidget::getDensitySettingsWidget()
{
    return _densitySettingsWidget;
}