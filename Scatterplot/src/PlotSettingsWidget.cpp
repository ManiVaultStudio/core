#include "PlotSettingsWidget.h"
#include "ScatterplotPlugin.h"
#include "PointSettingsWidget.h"
#include "DensitySettingsWidget.h"

using namespace hdps::gui;

PlotSettingsWidget::PlotSettingsWidget(QWidget* parent /*= nullptr*/) :
    ResponsiveToolBar::StatefulWidget(parent, "Plot"),
    _stackedWidget(new StackedWidget()),
    _pointSettingsWidget(new PointSettingsWidget(this)),
    _densitySettingsWidget(new DensitySettingsWidget(this))
{
    initializeUI();
}

void PlotSettingsWidget::initializeUI()
{
    _stackedWidget->addWidget(_pointSettingsWidget);
    _stackedWidget->addWidget(_densitySettingsWidget);
}

void PlotSettingsWidget::setScatterPlotPlugin(const ScatterplotPlugin& plugin)
{
    _pointSettingsWidget->setScatterPlotPlugin(plugin);
    _densitySettingsWidget->setScatterPlotPlugin(plugin);

    auto scatterplotWidget = const_cast<ScatterplotPlugin&>(plugin).getScatterplotWidget();

    const auto updatePlotSettingsUI = [this, scatterplotWidget]() {
        _stackedWidget->setCurrentIndex(scatterplotWidget->getRenderMode() == ScatterplotWidget::RenderMode::SCATTERPLOT ? 0 : 1);
    };

    connect(scatterplotWidget, &ScatterplotWidget::renderModeChanged, [this, updatePlotSettingsUI](const ScatterplotWidget::RenderMode& renderMode) {
        updatePlotSettingsUI();
    });

    updatePlotSettingsUI();
}

void PlotSettingsWidget::setState(const ResponsiveToolBar::WidgetState& state)
{
    StatefulWidget::setState(state);

    _pointSettingsWidget->setState(state);
    _densitySettingsWidget->setState(state);
}

QLayout* PlotSettingsWidget::getLayout(const ResponsiveToolBar::WidgetState& state)
{
    auto layout = new QHBoxLayout();

    layout->addWidget(_stackedWidget);

    return layout;
}