#include "SubsetSettingsWidget.h"
#include "ScatterplotPlugin.h"

#include "ui_SubsetSettingsWidget.h"

SubsetSettingsWidget::SubsetSettingsWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _ui{ std::make_unique<Ui::SubsetSettingsWidget>() }
{
    _ui->setupUi(this);
}

void SubsetSettingsWidget::initialize(const ScatterplotPlugin& plugin)
{
    QObject::connect(&plugin, qOverload<>(&ScatterplotPlugin::selectionChanged), [this]() {
    });
}