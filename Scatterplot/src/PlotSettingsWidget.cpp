#include "PlotSettingsWidget.h"
#include "ScatterplotPlugin.h"

#include "ui_PlotSettingsWidget.h"

PlotSettingsWidget::PlotSettingsWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _ui{ std::make_unique<Ui::PlotSettingsWidget>() }
{
    _ui->setupUi(this);
}

void PlotSettingsWidget::initialize(const ScatterplotPlugin& plugin)
{
    _ui->plotSettingsStackedWidget->initialize(plugin);

    /*
    const auto updateGroupBoxTitle = [this, &plugin]() {
        const auto renderMode = plugin._scatterPlotWidget->getRenderMode();

        switch (renderMode)
        {
            case ScatterplotWidget::RenderMode::SCATTERPLOT:
                _ui->groupBox->setTitle("Point");
                break;

            case ScatterplotWidget::RenderMode::DENSITY:
                _ui->groupBox->setTitle("Density");
                break;

            case ScatterplotWidget::RenderMode::LANDSCAPE:
                _ui->groupBox->setTitle("Contour");
                break;
                break;
        }
    };

    QObject::connect(plugin._scatterPlotWidget, &ScatterplotWidget::renderModeChanged, [this, updateGroupBoxTitle](const ScatterplotWidget::RenderMode& renderMode) {
        updateGroupBoxTitle();
    });

    updateGroupBoxTitle();
    */
}