#pragma once

#include <QWidget>

#include <QObject>
#include <QDebug>

class QWebEngineView;
class QWebEnginePage;
class QWebChannel;

namespace hdps
{
namespace gui
{

class WebCommunicationObject : public QObject
{
    Q_OBJECT

public:


signals:
    void notifyJsBridgeIsAvailable();

public slots:
    void js_debug(QString message)
    {
        qDebug() << "Javascript Debug Info: " << message;
    }

    void js_available()
    {
        emit notifyJsBridgeIsAvailable();
    }
};

class WebWidget : public QWidget
{
    Q_OBJECT
public:
    WebWidget();
    ~WebWidget() override;

    void init(WebCommunicationObject* communicationObject);

    QWebEngineView* getView();
    QWebEnginePage* getPage();
    void setPage(QString htmlPath, QString basePath);

protected:
    void registerFunctions();

public slots:
    void js_debug(QString text);

protected slots:
    virtual void initWebPage() = 0;

private:
    QWebEngineView* _webView;
    QWebChannel* _communicationChannel;

    WebCommunicationObject* _js;

    QString _css;
};

} // namespace gui

} // namespace hdps
