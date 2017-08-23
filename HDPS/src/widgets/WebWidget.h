#ifndef HDPS_GUI_WEB_WIDGET_H
#define HDPS_GUI_WEB_WIDGET_H

#include <QWidget>

class QWebView;
class QWebFrame;

namespace hdps
{
namespace gui
{

class WebWidget : public QWidget
{
    Q_OBJECT
public:
    WebWidget();
    ~WebWidget();

    void setPage(QString htmlPath, QString basePath);

protected:
    void registerFunctions(WebWidget* widget);

public slots:
    void js_debug(QString text);

protected slots:
    virtual void webViewLoaded(bool ok) = 0;
    virtual void connectJs() = 0;

private:
    QWebView* _webView;
    QWebFrame* _mainFrame;

    QString _css;
};

} // namespace gui

} // namespace hdps

#endif // HDPS_GUI_WEB_WIDGET_H
