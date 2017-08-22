#include "HeatMapWidget.h"

#include "util/FileUtil.h"

#include <QWebView>
#include <QWebFrame>

#include <QVBoxLayout>

#include <cassert>

HeatMapWidget::HeatMapWidget()
{
    Q_INIT_RESOURCE(resources);
}

HeatMapWidget::~HeatMapWidget()
{

}

void HeatMapWidget::setData(const std::vector<float>* data)
{

}

void HeatMapWidget::addSelectionListener(const hdps::plugin::SelectionListener* listener)
{

}

void HeatMapWidget::mousePressEvent(QMouseEvent *event)
{

}

void HeatMapWidget::mouseMoveEvent(QMouseEvent *event)
{

}

void HeatMapWidget::mouseReleaseEvent(QMouseEvent *event)
{

}

void HeatMapWidget::onSelection(QRectF selection)
{

}

void HeatMapWidget::cleanup()
{

}

void HeatMapWidget::connectJs()
{
    registerFunctions(this);
}
