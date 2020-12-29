#include "RenderSettingsWidget.h"
#include "ScatterplotPlugin.h"

#include "ui_RenderSettingsWidget.h"

RenderSettingsWidget::RenderSettingsWidget(const ScatterplotPlugin& plugin) :
    QWidget(),
    _ui{ std::make_unique<Ui::RenderSettingsWidget>() }
{
    _ui->setupUi(this);
}