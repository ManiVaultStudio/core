#ifndef D3_WIDGET_H
#define D3_WIDGET_H

#include "SelectionListener.h"

#include <QWidget>
#include <QMouseEvent>

class QWebView;
class QWebFrame;

class D3Widget : public QWidget
{
    Q_OBJECT
public:
    D3Widget();
    ~D3Widget();

    void setData(const std::vector<float>* data);
    void setColors(const std::vector<float>& data);

    void addSelectionListener(const hdps::plugin::SelectionListener* listener);
protected:
    void mousePressEvent(QMouseEvent *event)   Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event)    Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

    void onSelection(QRectF selection);
    void cleanup();

private slots:
    void webViewLoaded(bool ok);
    void connectJs();

private:
    unsigned int _numPoints = 0;
    const std::vector<float>* _positions;

    QSize _windowSize;

    QWebView* _webView;
    QWebFrame* _mainFrame;

    std::vector<const hdps::plugin::SelectionListener*> _selectionListeners;
};

#endif // D3_WIDGET_H
