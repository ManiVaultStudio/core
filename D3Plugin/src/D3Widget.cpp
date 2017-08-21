#include "D3Widget.h"

#include "util/FileUtil.h"

#include <QWebView>
#include <QWebFrame>

#include <QVBoxLayout>

#include <cassert>

D3Widget::D3Widget()
{
    _webView = new QWebView();
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(_webView);
    setLayout(layout);

    _mainFrame = _webView->page()->mainFrame();
    QObject::connect(_webView, &QWebView::loadFinished, this, &D3Widget::webViewLoaded);

    QObject::connect(_mainFrame, &QWebFrame::javaScriptWindowObjectCleared, this, &D3Widget::connectJs);

    Q_INIT_RESOURCE(resources);

    assert(_webView->settings()->testAttribute(QWebSettings::JavascriptEnabled));
    QString html = hdps::util::loadFileContents(":/test.html");
    qDebug() << html;
    _webView->setHtml(hdps::util::loadFileContents(":/test.html"), QUrl("qrc:/"));

    _css = "<style type=\"text/css\">" + hdps::util::loadFileContents("heatmap/heatmap.css") + "</style>";
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

void D3Widget::webViewLoaded(bool ok)
{

}

void D3Widget::connectJs()
{
    _mainFrame->addToJavaScriptWindowObject("Qt", this);
}
