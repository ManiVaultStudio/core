// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WebWidget.h"

#include "../util/FileUtil.h"

#include <QWebEngineView>
#include <QWebEnginePage>
#include <QWebChannel>

#include <QVBoxLayout>

#include <cassert>

namespace hdps
{
namespace gui
{

WebWidget::WebWidget()
{

}

WebWidget::~WebWidget()
{

}

void WebWidget::init(WebCommunicationObject* communicationObject)
{
    _js = communicationObject;
    QObject::connect(_js, &WebCommunicationObject::notifyJsBridgeIsAvailable, this, &WebWidget::initWebPage);

    _webView = new QWebEngineView();
    _webView->setAcceptDrops(false);
    //assert(_webView->settings()->testAttribute(QWebEngineSettings::JavascriptEnabled));

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(_webView);
    setLayout(layout);

    //QWebEnginePage* page = _webView->page();

    _communicationChannel = new QWebChannel();
    _webView->page()->setWebChannel(_communicationChannel);

    _communicationChannel->registerObject("QtBridge", _js);

    //QObject::connect(page, &QWebEnginePage::loadFinished, this, &WebWidget::initWebPage);
}

QWebEngineView* WebWidget::getView()
{
    return _webView;
}

QWebEnginePage* WebWidget::getPage()
{
    return _webView->page();
}

void WebWidget::setPage(QString htmlPath, QString basePath)
{
    //assert(_webView->settings()->testAttribute(QWebSettings::JavascriptEnabled));
    QString html = hdps::util::loadFileContents(htmlPath);

    _webView->setHtml(html, QUrl(basePath));
}

void WebWidget::registerFunctions()
{
    _communicationChannel->registerObject("QtBridge", _js);
}

void WebWidget::js_debug(QString text)
{
    qDebug() << "WebWidget Debug Info: " << text;
}

} // namespace gui

} // namespace hdps
