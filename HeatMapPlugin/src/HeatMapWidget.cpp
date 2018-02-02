#include "HeatMapWidget.h"

#include "ClustersPlugin.h"
#include "util/FileUtil.h"

#include <QWebView>
#include <QWebFrame>

#include <QVBoxLayout>

#include <cassert>

HeatMapWidget::HeatMapWidget()
    : loaded(false)
{
    Q_INIT_RESOURCE(resources);
}

HeatMapWidget::~HeatMapWidget()
{

}

void HeatMapWidget::addDataOption(const QString option)
{
    if (loaded)
        qt_addAvailableData(option);
    else
        dataOptionBuffer.append(option);
}

QString HeatMapWidget::getCurrentData() const
{
    return _currentData;
}

void HeatMapWidget::setData(const std::vector<Cluster>& clusters, const int numDimensions)
{
    std::string _jsonObject = "";

    _numClusters = clusters.size();
    qDebug() << "Setting data";
    //// Nodes
    std::string nodes = "\"nodes\":[\n";
    for (int i = 0; i < _numClusters; i++)
    {
        nodes = nodes + "{\"name\":\"" + "Cluster name " + std::to_string(i) + "\", ";
        nodes = nodes + "\"size\":" + std::to_string(_numClusters) + ", ";
        nodes = nodes + "\"expression\":[";

        for (int j = 0; j < numDimensions; j++)
        {
            nodes = nodes + std::to_string(clusters[i]._mean[j]);

            if (j < numDimensions - 1) nodes = nodes + ",";
        }
        nodes = nodes + "], \"stddev\":[";
        for (int j = 0; j < numDimensions; j++)
        {
            nodes = nodes + std::to_string(clusters[i]._stddev[j]);

            if (j < numDimensions - 1) nodes = nodes + ",";
        }
        if (i < _numClusters - 1) nodes = nodes + "]},\n";
        else nodes = nodes + "]}\n]";
    }

    // TODO: multi files
    std::string names = ",\n\"names\":[";
    for (int i = 0; i < numDimensions; i++) {

        names = names + "\"" + "markerName " + std::to_string(i) + "\"";

        if (i < numDimensions - 1) names = names + ",";
        else names = names + "]";
    }

    _jsonObject = "{\n" + nodes + names + "\n}";

    qDebug() << _jsonObject.c_str();

    qt_setData(QString(_jsonObject.c_str()));
}

void HeatMapWidget::mousePressEvent(QMouseEvent *event)
{
    // UNUSED
}

void HeatMapWidget::mouseMoveEvent(QMouseEvent *event)
{
    // UNUSED
}

void HeatMapWidget::mouseReleaseEvent(QMouseEvent *event)
{
    // UNUSED
}

void HeatMapWidget::onSelection(QRectF selection)
{
    // UNUSED
}

void HeatMapWidget::cleanup()
{

}

void HeatMapWidget::connectJs()
{
    qDebug() << "CONNECT JS";
    registerFunctions(this);
}

void HeatMapWidget::webViewLoaded(bool ok)
{
    loaded = true;

    for (QString option : dataOptionBuffer) {
        qt_addAvailableData(option);
    }
    
    qDebug() << "HEATMAP LOADED!" << ok;
}

void HeatMapWidget::js_selectData(QString name)
{
    _currentData = name;

    emit dataSetPicked(name);
}

void HeatMapWidget::js_highlightUpdated(int highlightId)
{
    QList<int> selectedClusters;

    for (int i = 0; i < _numClusters; i++) {
        selectedClusters.append(i == highlightId ? 1 : 0);
    }

    selectedClusters.append(highlightId);

    //emit clusterSelectionChanged(selectedClusters);
}

void HeatMapWidget::js_selectionUpdated(QList<int> selectedClusters)
{
    emit clusterSelectionChanged(selectedClusters);
}
