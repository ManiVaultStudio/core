#include "WebWidget.h"

#include "../util/FileUtil.h"

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

void WebWidget::registerFunctions(WebWidget* widget)
{
    _mainFrame->addToJavaScriptWindowObject("Qt", widget);
}

void WebWidget::js_debug(QString text)
{
    qDebug() << "WebWidget Debug Info: " << text;
}

} // namespace gui

} // namespace hdps
