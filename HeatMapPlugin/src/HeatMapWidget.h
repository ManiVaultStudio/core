#ifndef HEAT_MAP_WIDGET_H
#define HEAT_MAP_WIDGET_H

#include "SelectionListener.h"

#include "widgets/WebWidget.h"
#include <QMouseEvent>
#include <QComboBox>
#include <QList>

class QWebView;
class QWebFrame;

struct Cluster;

class HeatMapWidget : public hdps::gui::WebWidget
{
    Q_OBJECT
public:
    HeatMapWidget();
    ~HeatMapWidget();

    void addDataOption(const QString option);
    QString getCurrentData() const;
    void setData(const std::vector<Cluster>& data, const int numDimensions);

protected:
    void mousePressEvent(QMouseEvent *event)   Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event)    Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

    void onSelection(QRectF selection);
    void cleanup();

signals:
    void clusterSelectionChanged(QList<int> selectedClusters);
    void dataSetPicked(QString name);

    void qt_setData(QString data);
    void qt_addAvailableData(QString name);
    void qt_setSelection(QList<int> selection);
    void qt_setHighlight(int highlightId);
    void qt_setMarkerSelection(QList<int> selection);

private slots:
    virtual void connectJs() override;
    virtual void webViewLoaded(bool ok) override;

public slots:
    void js_selectData(QString text);
    void js_selectionUpdated(QList<int> selectedClusters);
    void js_highlightUpdated(int highlightId);

private:
    QString _currentData;
    unsigned int _numClusters;

    /** Whether the web view has loaded and web-functions are ready to be called. */
    bool loaded;
    /** Temporary storage for added data options until webview is loaded */
    QList<QString> dataOptionBuffer;
};

#endif // HEAT_MAP_WIDGET_H
