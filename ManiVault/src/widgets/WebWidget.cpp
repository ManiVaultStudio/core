// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WebWidget.h"

#include "../util/FileUtil.h"

#include <QWebEngineView>
#include <QWebEnginePage>
#include <QWebChannel>
#include <QDebug>
#include <QVBoxLayout>

#include <cassert>

namespace mv
{
namespace gui
{

void WebCommunicationObject::js_debug(const QString& text)
{
    qDebug() << "WebWidget Debug Info: " << text;
}

void WebWidget::init(WebCommunicationObject* communicationObject)
{
    _webCommunicationObject = communicationObject;
    connect(_webCommunicationObject, &WebCommunicationObject::notifyJsBridgeIsAvailable, this, &WebWidget::onJsBridgeIsAvailable);

    // DEPRECATED, to be removed in any release after 2.0
    connect(_webCommunicationObject, &WebCommunicationObject::notifyJsBridgeIsAvailable, this, &WebWidget::initWebPage);

    _webView = new QWebEngineView();
    _webView->setAcceptDrops(false);

    connect(_webView, &QWebEngineView::loadFinished, this, &WebWidget::onWebPageLoaded);
    //assert(_webView->settings()->testAttribute(QWebEngineSettings::JavascriptEnabled));

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(_webView);
    setLayout(layout);

    //QWebEnginePage* page = _webView->page();

    _communicationChannel = new QWebChannel();
    _webView->page()->setWebChannel(_communicationChannel);

    _communicationChannel->registerObject("QtBridge", _webCommunicationObject);
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
    QString html = mv::util::loadFileContents(htmlPath);

    _webView->setHtml(html, QUrl(basePath));
}

void WebWidget::registerFunctions()
{
    _communicationChannel->registerObject("QtBridge", _webCommunicationObject);
}

void WebWidget::onJsBridgeIsAvailable()
{
    qDebug() << "WebWidget: Web page communication available.";
    _communicationAvailable = true;

    emit communicationBridgeReady();
    if (_webPageLoaded)
        emit webPageFullyLoaded();
}

void WebWidget::onWebPageLoaded(bool ok)
{
    if (ok)
    {
        qDebug() << "WebWidget: Web page finished loading.";
        _webPageLoaded = true;

        if (_communicationAvailable)
            emit webPageFullyLoaded();
    }
    else
        qWarning() << "WebWidget: Web page failed to load properly.";
}

} // namespace gui

} // namespace mv
