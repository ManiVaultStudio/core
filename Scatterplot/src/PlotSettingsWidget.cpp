#include "PlotSettingsWidget.h"
#include "ScatterplotPlugin.h"
#include "PointSettingsWidget.h"
#include "DensitySettingsWidget.h"

using namespace hdps::gui;

PlotSettingsWidget::PlotSettingsWidget(const ResponsiveSectionWidget::WidgetState& widgetState, QWidget* parent /*= nullptr*/) :
    ScatterplotSettingsWidget(widgetState, parent),
    _stackedWidget(new StackedWidget()),
    _pointSettingsWidget(new PointSettingsWidget(widgetState, this)),
    _densitySettingsWidget(new DensitySettingsWidget(widgetState, this))
{
    _stackedWidget->addWidget(_pointSettingsWidget);
    _stackedWidget->addWidget(_densitySettingsWidget);

    auto layout = new QHBoxLayout();

    applyLayout(layout);

    layout->addWidget(_stackedWidget);

    setLayout(layout);
}

void PlotSettingsWidget::setScatterplotPlugin(ScatterplotPlugin* scatterplotPlugin)
{
    _scatterplotPlugin = scatterplotPlugin;

    _pointSettingsWidget->setScatterplotPlugin(scatterplotPlugin);
    _densitySettingsWidget->setScatterplotPlugin(scatterplotPlugin);

    const auto updatePlotSettingsUI = [this]() {
        _stackedWidget->setCurrentIndex(_scatterplotPlugin->getScatterplotWidget()->getRenderMode() == ScatterplotWidget::RenderMode::SCATTERPLOT ? 0 : 1);
    };
     
    connect(_scatterplotPlugin->getScatterplotWidget(), &ScatterplotWidget::renderModeChanged, this, [this, updatePlotSettingsUI](const ScatterplotWidget::RenderMode& renderMode) {
        updatePlotSettingsUI();
    });
    
    updatePlotSettingsUI();
}