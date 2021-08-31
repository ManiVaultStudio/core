#include "ClusterData.h"
#include "InfoAction.h"

#include "Application.h"

#include <QtCore>
#include <QtDebug>

Q_PLUGIN_METADATA(IID "hdps.ClusterData")

using namespace hdps::util;

ClusterData::ClusterData(const hdps::plugin::PluginFactory* factory) :
    hdps::plugin::RawData(factory, ClusterType),
    Visitable()
{
}

ClusterData::~ClusterData(void)
{
}

void ClusterData::init()
{
}

hdps::DataSet* ClusterData::createDataSet() const
{
    return new Clusters(_core, getName());
}

std::vector<Cluster>& ClusterData::getClusters()
{
    return _clusters;
}

void ClusterData::accept(ClusterDataVisitor& visitor) const
{

}

void ClusterData::addCluster(Cluster& cluster)
{
    _clusters.push_back(cluster);
}

void ClusterData::removeClusterById(const QString& id)
{
    _clusters.erase(std::remove_if(_clusters.begin(), _clusters.end(), [id](const Cluster& cluster) -> bool
    {
        return cluster.getId() == id;
    }), _clusters.end());
}

void ClusterData::removeClustersById(const QStringList& ids)
{
    for (auto& clusterId : ids)
        removeClusterById(clusterId);
}

// =============================================================================
// Factory
// =============================================================================

hdps::plugin::RawData* ClusterDataFactory::produce()
{
    return new ClusterData(this);
}

void Clusters::init()
{
    _infoAction = QSharedPointer<InfoAction>::create(nullptr, _core, getName());

    addAction(*_infoAction.get());
}

void Clusters::addCluster(Cluster& cluster)
{
    getRawData<ClusterData>().addCluster(cluster);
}

void Clusters::removeClusterById(const QString& id)
{
    getRawData<ClusterData>().removeClusterById(id);
}

void Clusters::removeClustersById(const QStringList& ids)
{
    getRawData<ClusterData>().removeClustersById(ids);
}

QIcon Clusters::getIcon() const
{
    return hdps::Application::getIconFont("FontAwesome").getIcon("th-large");
}


//QSharedPointer<ClusterAction>   _clusterAction;     /** Shared pointer to cluster action */
//_clusterAction = QSharedPointer<ClusterAction>::create(nullptr, _core, getName());