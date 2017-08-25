#ifndef HEAT_MAP_WIDGET_H
#define HEAT_MAP_WIDGET_H

#include "SelectionListener.h"

#include "widgets/WebWidget.h"
#include <QMouseEvent>

class QWebView;
class QWebFrame;

class HeatMapWidget : public hdps::gui::WebWidget
{
    Q_OBJECT
public:
    HeatMapWidget();
    ~HeatMapWidget();

    void setData(const std::vector<float>* data);

    void addSelectionListener(const hdps::plugin::SelectionListener* listener);
protected:
    void mousePressEvent(QMouseEvent *event)   Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event)    Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

    void onSelection(QRectF selection);
    void cleanup();

private:
    unsigned int _numPoints = 0;
    const std::vector<float>* _positions;

    QSize _windowSize;

    std::vector<const hdps::plugin::SelectionListener*> _selectionListeners;

private slots:
    virtual void connectJs() override;
    virtual void webViewLoaded(bool ok) override;

public slots:
    //void js_selectData(QString text);
    void js_selectionUpdated(QList<int> selectedClusters);
    void js_highlightUpdated(int highlightId);
signals:
    void qt_setData(QString data);
    void qt_addAvailableData(QString name);
    void qt_setSelection(QList<int> selection);
    void qt_setHighlight(int highlightId);
    void qt_setMarkerSelection(QList<int> selection);
};

#endif // HEAT_MAP_WIDGET_H
