// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QWidget>

#include <QString>
#include <QObject>

class QWebEngineView;
class QWebEnginePage;
class QWebChannel;

namespace mv
{
namespace gui
{

class CORE_EXPORT WebCommunicationObject : public QObject
{
    Q_OBJECT

signals:
    void notifyJsBridgeIsAvailable();

public slots:
    void js_debug(const QString& message);

    void js_available()
    {
        emit notifyJsBridgeIsAvailable();
    }
};

class CORE_EXPORT WebWidget : public QWidget
{
    Q_OBJECT
public:
    WebWidget();

    void init(WebCommunicationObject* communicationObject);

public: // setter
    void setPage(QString htmlPath, QString basePath);

public: // getter
    QWebEngineView* getView();
    QWebEnginePage* getPage();

    bool isWebPageLoaded() const { return _webPageLoaded; }

signals:
    void communicationBridgeReady();    // emitted when the communication bridge is ready; the webpage is not necessarily loaded at this point
    void webPageFullyLoaded();          // emitted after both the communication bridge is ready and the webpage is fully loaded

protected:
    void registerFunctions();

protected slots:
    [[deprecated("Will be removed in 2.0. Connect to the communicationBridgeReady() or webPageFullyLoaded() signals instead.")]]
    virtual void initWebPage() {}

private slots:
    void onJsBridgeIsAvailable();
    void onWebPageLoaded(bool ok);

private:
    QWebEngineView* _webView;
    QWebChannel* _communicationChannel;

    WebCommunicationObject* _webCommunicationObject;

    QString _css;

    bool _communicationAvailable;
    bool _webPageLoaded;
};

} // namespace gui

} // namespace mv
