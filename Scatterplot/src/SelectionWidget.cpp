#include "SelectionWidget.h"

#include "Application.h"

#include "ui_SelectionWidget.h"

#include <QDebug>

SelectionWidget::SelectionWidget(QWidget* parent) :
	_ui{ std::make_unique<Ui::SelectionWidget>() }
{
    _ui->setupUi(this);
}