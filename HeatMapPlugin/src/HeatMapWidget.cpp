#include "HeatMapWidget.h"

#include "ClustersPlugin.h"
#include "util/FileUtil.h"

#include <QWebView>
#include <QWebFrame>

#include <QVBoxLayout>

#include <cassert>

HeatMapWidget::HeatMapWidget()
{
    Q_INIT_RESOURCE(resources);
}

HeatMapWidget::~HeatMapWidget()
{

}

void HeatMapWidget::setData(const std::vector<Cluster>& clusters, const int numDimensions)
{
    //if (((MCV_DerivedData*)data)->type() == 2)
    //{
    //    qt_addAvailableData(QString(((MCV_DerivedData*)data)->name().c_str()));
    //}

    //if ((void*)_data != data) return;

    //qt_setData(QString(_data->jsonObject()->c_str()));


    //// Clusters array, one cluster is vector of floats containing x, y coordinates of points sequentially
    //
    //std::vector<float> clusters[7];

    //for (int i = 0; i < data.size(); i += 3)
    //{
    //    float x = data[i + 0];
    //    float y = data[i + 1];
    //    int c = data[i + 2];

    //    clusters[c].push_back(x * 0.5 + 0.5);
    //    clusters[c].push_back(y * 0.5 + 0.5);
    //}

    //// Mean expressions of each cluster for each dimension
    //std::vector<float> expressions[2];
    //// Standard deviation of each cluster for each dimension
    //std::vector<float> std_devs[2];

    //for (int i = 0; i < 7; i++) {
    //    float mean = 0;
    //    int count = 0;
    //    for (int j = 0; j < clusters[i].size(); j += 2) {
    //        mean += clusters[i][j];
    //        count++;
    //    }
    //    expressions[0].push_back(mean / count);

    //    mean = 0;
    //    count = 0;
    //    for (int j = 1; j < clusters[i].size(); j += 2) {
    //        mean += clusters[i][j];
    //        count++;
    //    }
    //    expressions[1].push_back(mean / count);
    //}

    //for (int i = 0; i < 7; i++) {
    //    float variance = 0;
    //    int count = 0;
    //    for (int j = 0; j < clusters[i].size(); j += 2) {
    //        variance += pow(clusters[i][j] - expressions[0][i], 2);
    //        count++;
    //    }
    //    std_devs[0].push_back(sqrt(variance / count));

    //    variance = 0;
    //    count = 0;
    //    for (int j = 1; j < clusters[i].size(); j += 2) {
    //        variance += pow(clusters[i][j] - expressions[1][i], 2);
    //        count++;
    //    }
    //    std_devs[1].push_back(sqrt(variance / count));
    //}

    std::string _jsonObject = "";

    unsigned int numClusters = clusters.size();
    qDebug() << "Setting data";
    //// Nodes
    std::string nodes = "\"nodes\":[\n";
    for (int i = 0; i < numClusters; i++)
    {
        nodes = nodes + "{\"name\":\"" + "Cluster name " + std::to_string(i) + "\", ";
        nodes = nodes + "\"size\":" + std::to_string(numClusters) + ", ";
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
        if (i < numClusters - 1) nodes = nodes + "]},\n";
        else nodes = nodes + "]}\n]";
    }

    //// Edges
    std::string edges = "";

    // TODO: multi files
    std::string names = ",\n\"names\":[";
    for (int i = 0; i < numDimensions; i++) {

        names = names + "\"" + "markerName " + std::to_string(i) + "\"";

        if (i < numDimensions - 1) names = names + ",";
        else names = names + "]";
    }

    _jsonObject = "{\n" + nodes + edges + names + "\n}";


    qt_addAvailableData(QString("Density Clusters - A-tSNE Analysis"));

    //QString json = hdps::util::loadFileContents("cluster2.json");

    qDebug() << _jsonObject.c_str();

    qt_setData(QString(_jsonObject.c_str()));
}

void HeatMapWidget::addSelectionListener(const hdps::plugin::SelectionListener* listener)
{

}

void HeatMapWidget::mousePressEvent(QMouseEvent *event)
{

}

void HeatMapWidget::mouseMoveEvent(QMouseEvent *event)
{

}

void HeatMapWidget::mouseReleaseEvent(QMouseEvent *event)
{

}

void HeatMapWidget::onSelection(QRectF selection)
{

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
    qDebug() << "HEATMAP LOADED!" << ok;
}

void HeatMapWidget::js_highlightUpdated(int highlightId)
{
    //if (!_data) return;
    qDebug() << "Highlight";
    //_data->setHighlight(highlightId);
}

void HeatMapWidget::js_selectionUpdated(QList<int> selectedClusters)
{
    qDebug() << selectedClusters;
}
