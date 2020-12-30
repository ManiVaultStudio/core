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
}