#include "RenderModeWidget.h"
#include "ScatterplotPlugin.h"

#include "ui_RenderModeWidget.h"

RenderModeWidget::RenderModeWidget(const ScatterplotPlugin& plugin) :
    QWidget(),
    _ui{ std::make_unique<Ui::RenderModeWidget>() }
{
    _ui->setupUi(this);
}