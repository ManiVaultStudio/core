#include "D3Widget.h"

#include <QWebView>
#include <QWebFrame>

#include <cassert>

D3Widget::D3Widget()
{
    _webView = new QWebView(this);

    QObject::connect(_webView, &QWebView::loadFinished, this, &D3Widget::webViewLoaded);

    QObject::connect(_mainFrame, &QWebFrame::javaScriptWindowObjectCleared, this, &D3Widget::connectJs);

    assert(_webView->settings()->testAttribute(QWebSettings::JavascriptEnabled));

    QFile file("test.html");
    QString html = "";
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        html = stream.readAll();
    }

    _webView->setHtml(html, QUrl(""));
}

D3Widget::~D3Widget()
{

}

void D3Widget::setData(const std::vector<float>* data)
{

}

void D3Widget::setColors(const std::vector<float>& data)
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

void D3Widget::webViewLoaded(bool ok)
{

}

void D3Widget::connectJs()
{

}
