#include "MeanShiftClusteringPlugin.h"

#include "PointsPlugin.h"
#include "ClustersPlugin.h"

#include <QtCore>
#include <QMessageBox>
#include <QtDebug>


#include <assert.h>
#include <algorithm>
#include <functional>
#include <random>
#include <vector>

#include <unordered_map>

#define NO_FILE 0

Q_PLUGIN_METADATA(IID "nl.tudelft.ClusteringPlugin")

// =============================================================================
// View
// =============================================================================

MeanShiftAnalysisPlugin::~MeanShiftAnalysisPlugin(void)
{
    
}

void MeanShiftAnalysisPlugin::init()
{
    connect(&_settings, SIGNAL(startClustering()), this, SLOT(startComputation()));

    offscreen.bindContext();
    _meanShift.init();
    offscreen.releaseContext();
}

void MeanShiftAnalysisPlugin::dataAdded(const QString name)
{
    _settings.addDataOption(name);
}

void MeanShiftAnalysisPlugin::dataChanged(const QString name)
{

}

void MeanShiftAnalysisPlugin::dataRemoved(const QString name)
{
    
}

void MeanShiftAnalysisPlugin::selectionChanged(const QString dataName)
{

}


QStringList MeanShiftAnalysisPlugin::supportedDataKinds()
{
    QStringList supportedKinds;
    supportedKinds << "Points";
    return supportedKinds;
}

SettingsWidget* const MeanShiftAnalysisPlugin::getSettings()
{
    return &_settings;
}

void MeanShiftAnalysisPlugin::dataSetPicked(const QString& name)
{

}

void MeanShiftAnalysisPlugin::startComputation()
{
    QString setName = _settings.getCurrentDataOption();

    // Do nothing if we have no data set selected
    if (setName.isEmpty()) {
        QMessageBox warning;
        warning.setText("No data is selected for clustering.");
        warning.exec();
        return;
    }

    IndexSet& set = dynamic_cast<IndexSet&>(_core->requestSet(setName));
    PointsPlugin& points = set.getData();

    if (points.getNumDimensions() != 2)
    {
        QMessageBox warning;
        warning.setText("Selected data must be 2-dimensional.");
        warning.exec();
        return;
    }

    std::vector<hdps::Vector2f>* points2D = (std::vector<hdps::Vector2f>*) &points.getData();

    _meanShift.setData(points2D);

    std::vector<std::vector<unsigned int>> clusters;

    offscreen.bindContext();
    _meanShift.cluster(*points2D, clusters);
    offscreen.releaseContext();

    QString clusterSetName = _core->addData("Clusters", "ClusterSet");
    const ClusterSet& clusterSet = dynamic_cast<ClusterSet&>(_core->requestSet(clusterSetName));
    ClustersPlugin& plugin = clusterSet.getData();

    for (auto c : clusters)
    {
        IndexSet* cluster = (IndexSet*)points.createSet();
        for (int j = 0; j < c.size(); j++)
        {
            cluster->indices.push_back(c[j]);
        }
        plugin.addCluster(cluster);
    }

    _core->notifyDataAdded(clusterSetName);
}

// =============================================================================
// Factory
// =============================================================================

AnalysisPlugin* MeanShiftAnalysisPluginFactory::produce()
{
    return new MeanShiftAnalysisPlugin();
}
