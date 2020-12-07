#include "SelectionWidget.h"

#include "Application.h"

#include "ui_SelectionWidget.h"

#include <QDebug>
#include <QImage>

SelectionLayerWidget::SelectionLayerWidget(QWidget* parent) :
	_ui{ std::make_unique<Ui::SelectionWidget>() }
{
}