#include "WebWidget.h"

#include "util/FileUtil.h"

#include <QWebView>
#include <QWebFrame>

#include <QVBoxLayout>

#include <cassert>

namespace hdps
{
namespace gui
{

WebWidget::WebWidget()
{
    _webView = new QWebView();
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(_webView);
    setLayout(layout);

    _mainFrame = _webView->page()->mainFrame();
    QObject::connect(_webView, &QWebView::loadFinished, this, &WebWidget::webViewLoaded);

    QObject::connect(_mainFrame, &QWebFrame::javaScriptWindowObjectCleared, this, &WebWidget::connectJs);
}

WebWidget::~WebWidget()
{

}

void WebWidget::setPage(QString htmlPath, QString basePath)
{
    assert(_webView->settings()->testAttribute(QWebSettings::JavascriptEnabled));

    _webView->setHtml(hdps::util::loadFileContents(htmlPath), QUrl(basePath));
}

void WebWidget::webViewLoaded(bool ok)
{

}

void WebWidget::connectJs()
{
    _mainFrame->addToJavaScriptWindowObject("Qt", this);
}

} // namespace gui

} // namespace hdps
