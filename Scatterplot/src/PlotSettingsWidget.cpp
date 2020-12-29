#include "PlotSettingsWidget.h"
#include "ScatterplotPlugin.h"

#include "ui_PlotSettingsWidget.h"

PlotSettingsWidget::PlotSettingsWidget(const ScatterplotPlugin& plugin) :
    QWidget(),
    _ui{ std::make_unique<Ui::PlotSettingsWidget>() }
{
    _ui->setupUi(this);
}