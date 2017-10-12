#include "SpadeAnalysisPlugin.h"

#include "PointsPlugin.h"
#include "ClustersPlugin.h"

#include <QtCore>
#include <QMessageBox>
#include <QtDebug>

#include <unordered_map>

Q_PLUGIN_METADATA(IID "nl.tudelft.ClusteringPlugin")

// =============================================================================
// View
// =============================================================================

SpadeAnalysisPlugin::~SpadeAnalysisPlugin(void)
{
    
}

void SpadeAnalysisPlugin::init()
{
    _settings = std::make_unique<SpadeSettingsWidget>(this);
}

void SpadeAnalysisPlugin::dataAdded(const QString name)
{
    _settings->dataOptions.addItem(name);
}

void SpadeAnalysisPlugin::dataChanged(const QString name)
{

}

void SpadeAnalysisPlugin::dataRemoved(const QString name)
{
    
}

void SpadeAnalysisPlugin::selectionChanged(const QString dataName)
{

}


QStringList SpadeAnalysisPlugin::supportedDataKinds()
{
    QStringList supportedKinds;
    supportedKinds << "Points";
    return supportedKinds;
}

SettingsWidget* const SpadeAnalysisPlugin::getSettings()
{
    return _settings.get();
}

void SpadeAnalysisPlugin::dataSetPicked(const QString& name)
{

}

void SpadeAnalysisPlugin::startComputation()
{
    QString setName = _settings->dataOptions.currentText();

    // Do nothing if we have no data set selected
    if (setName.isEmpty()) {
        return;
    }

    const hdps::Set* set = _core->requestData(setName);
    const DataTypePlugin* dataPlugin = _core->requestPlugin(set->getDataName());
    const PointsPlugin* points = dynamic_cast<const PointsPlugin*>(dataPlugin);

    // Clustering
    IndexSet* set1 = (IndexSet*)points->createSet();
    IndexSet* set2 = (IndexSet*)points->createSet();
    std::unordered_map<int, IndexSet*> clusters;

    unsigned int numPoints = points->data.size() / points->numDimensions;
    for (int i = 0; i < numPoints; i++)
    {
        int c = (int) points->data[i * points->numDimensions + points->numDimensions - 1];

        if (clusters.find(c) == clusters.end())
        {
            clusters[c] = (IndexSet*) points->createSet();
        }
        clusters[c]->indices.push_back(i);
    }

    QString clusterSetName = _core->addData("Clusters", "ClusterSet");
    const ClusterSet* clusterSet = dynamic_cast<ClusterSet*>(_core->requestData(clusterSetName));
    ClustersPlugin* plugin = dynamic_cast<ClustersPlugin*>(_core->requestPlugin(clusterSet->getDataName()));

    for (auto& it : clusters)
    {
        IndexSet* cluster = it.second;
        cluster->setDataName(points->getName());
        plugin->addCluster(cluster);
    }

    _core->notifyDataAdded(clusterSetName);
}

// =============================================================================
// Factory
// =============================================================================

AnalysisPlugin* SpadeAnalysisPluginFactory::produce()
{
    return new SpadeAnalysisPlugin();
}
