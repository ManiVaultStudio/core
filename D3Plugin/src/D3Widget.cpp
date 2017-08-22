#include "D3Widget.h"

#include "util/FileUtil.h"

#include <QWebView>
#include <QWebFrame>

#include <QVBoxLayout>

#include <cassert>

D3Widget::D3Widget()
{
    Q_INIT_RESOURCE(resources);
}

D3Widget::~D3Widget()
{

}

void D3Widget::setData(const std::vector<float>* data)
{

}

void D3Widget::addSelectionListener(const hdps::plugin::SelectionListener* listener)
{

}

void D3Widget::mousePressEvent(QMouseEvent *event)
{

}

void D3Widget::mouseMoveEvent(QMouseEvent *event)
{

}

void D3Widget::mouseReleaseEvent(QMouseEvent *event)
{

}

void D3Widget::onSelection(QRectF selection)
{

}

void D3Widget::cleanup()
{

}

