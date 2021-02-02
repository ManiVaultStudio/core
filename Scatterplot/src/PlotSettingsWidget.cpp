#include "PlotSettingsWidget.h"
#include "ScatterplotPlugin.h"
#include "PointSettingsWidget.h"
#include "DensitySettingsWidget.h"

using namespace hdps::gui;

PlotSettingsWidget::PlotSettingsWidget(const hdps::gui::ResponsiveSectionWidget::State& state, QWidget* parent /*= nullptr*/) :
    ScatterplotSettingsWidget(state, parent),
    _stackedWidget(new StackedWidget()),
    _pointSettingsWidget(new PointSettingsWidget(state, this)),
    _densitySettingsWidget(new DensitySettingsWidget(state, this))
{
    //qDebug() << QString("PlotSettingsWidget::PlotSettingsWidget(state=%1)").arg(QString::number(static_cast<std::int32_t>(state)));

    _stackedWidget->addWidget(_pointSettingsWidget);
    _stackedWidget->addWidget(_densitySettingsWidget);

    auto layout = new QHBoxLayout();

    layout->setMargin(0);
    layout->setSpacing(4);

    layout->addWidget(_stackedWidget);

    setLayout(layout);
}

PlotSettingsWidget::~PlotSettingsWidget()
{
    qDebug() << "Delete" << objectName();
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
        qDebug() << "PlotSettingsWidget::renderModeChanged()";
        updatePlotSettingsUI();
        auto test = "asdasd";
        qDebug() << test;
    });
    
    updatePlotSettingsUI();
}