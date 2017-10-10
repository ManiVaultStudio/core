#include "HeatMapPlugin.h"

#include "HeatMapWidget.h"
#include "ClustersPlugin.h"

#include <QtCore>
#include <QtDebug>

Q_PLUGIN_METADATA(IID "nl.tudelft.HeatMapPlugin")

// =============================================================================
// View
// =============================================================================

HeatMapPlugin::~HeatMapPlugin(void)
{
    
}

void HeatMapPlugin::init()
{
    heatmap = new HeatMapWidget();
    heatmap->setPage(":/heatmap/heatmap.html", "qrc:/heatmap/");
    heatmap->addSelectionListener(this);

    addWidget(heatmap);

    //QString clusterName = _core->createClusterSet("SPADE Cluster");
}

void HeatMapPlugin::dataAdded(const QString name)
{
    qDebug() << "Attempting cast to ClusterSet";
    ClusterSet* clusterSet = dynamic_cast<ClusterSet*>(_core->requestData(name));

    if (!clusterSet) return;
    qDebug() << "Requesting plugin";
    const ClustersPlugin* clusterPlugin = dynamic_cast<const ClustersPlugin*>(_core->requestPlugin(clusterSet->getDataName()));

    qDebug() << "DATA SIZE: " << clusterPlugin->clusters.size();

    const std::vector<float>& data = points->data;
    qDebug() << "Calculating data";
    ////////////
    //for (int i = 0; i < data.size() / points->numDimensions; i++)
    //{
    //    int cluster = data[i * points->numDimensions + points->numDimensions-1];

    //    if (clusterSet->clusters.size() < cluster + 1)
    //        clusterSet->clusters.resize(cluster + 1, Cluster());

    //    clusterSet->clusters[cluster].indices.push_back(i);
    //}

    int numClusters = clusterPlugin->clusters.size();

    // For every cluster initialize the median, mean, and stddev vectors with the number of dimensions
    for (IndexSet* cluster : clusterPlugin->clusters) {
        cluster._median.resize(points->numDimensions);
        cluster._mean.resize(points->numDimensions);
        cluster._stddev.resize(points->numDimensions);
    }

    for (Cluster& cluster : clusterSet->clusters)
    {
        // FIXME remove -1 from iteration condition, we assume the last element is the clusterID here
        for (int d = 0; d < points->numDimensions - 1; d++)
        {
            // Mean calculation
            float mean = 0;

            for (int index : cluster.indices)
                mean += data[index * points->numDimensions + d];

            mean /= cluster.indices.size();

            // Standard deviation calculation
            float variance = 0;

            for (int index : cluster.indices)
                variance += pow(data[index * points->numDimensions + d] - mean, 2);

            float stddev = sqrt(variance / cluster.indices.size());

            cluster._mean[d] = mean;
            cluster._stddev[d] = stddev;
        }
    }

    /////////////
    qDebug() << "Done calculating data";


    heatmap->setData(clusterSet->clusters, points->numDimensions);
}

void HeatMapPlugin::dataChanged(const QString name)
{

}

void HeatMapPlugin::dataRemoved(const QString name)
{
    
}

void HeatMapPlugin::selectionChanged(const QString dataName)
{

}

QStringList HeatMapPlugin::supportedDataKinds()
{
    QStringList supportedKinds;
    supportedKinds << "Clusters";
    return supportedKinds;
}

void HeatMapPlugin::dataSetPicked(const QString& name)
{

}

void HeatMapPlugin::updateData()
{

}

void HeatMapPlugin::onSelection(const std::vector<unsigned int> selection) const
{

}

// =============================================================================
// Factory
// =============================================================================

ViewPlugin* HeatMapPluginFactory::produce()
{
    return new HeatMapPlugin();
}
