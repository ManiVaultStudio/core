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

void ClusterData::accept(ClusterDataVisitor* visitor) const
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

void Clusters::init()
{
    _infoAction = QSharedPointer<InfoAction>::create(nullptr, getName());

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
    return Application::getIconFont("FontAwesome").getIcon("th-large");
}

void Clusters::fromVariant(const QVariant& variant)
{
    if (variant.type() != QVariant::Type::List)
        throw std::runtime_error("Clusters variant is not a list");

    std::vector<Cluster>& clusters = getClusters();

    const auto variantList = variant.toList();

    clusters.clear();
    clusters.resize(variantList.count());

    auto clusterIndex = 0;

    for (auto& cluster : clusters) {
        cluster.fromVariant(variantList.at(clusterIndex));
        clusterIndex++;
    }
}

QVariant Clusters::toVariant() const
{
    QVariantList clustersList;

    for (auto cluster : getClusters())
        clustersList << cluster.toVariant();

    return clustersList;
}

std::vector<std::uint32_t> Clusters::getSelectedIndices() const
{
    // Indices that are selected
    std::vector<std::uint32_t> selectedIndices;

    // Gather indices
    for (const auto& clusterIndex : getSelection<Clusters>().indices) {

        // Get reference to selected cluster
        const auto selectedCluster = getClusters()[clusterIndex];

        // Add cluster indices to selected indices
        selectedIndices.insert(selectedIndices.end(), selectedCluster.getIndices().begin(), selectedCluster.getIndices().end());
    }

    // Remove duplicates
    std::sort(selectedIndices.begin(), selectedIndices.end());
    selectedIndices.erase(unique(selectedIndices.begin(), selectedIndices.end()), selectedIndices.end());

    return selectedIndices;
}

QIcon ClusterDataFactory::getIcon() const
{
    return Application::getIconFont("FontAwesome").getIcon("th-large");
}

hdps::plugin::RawData* ClusterDataFactory::produce()
{
    return new ClusterData(this);
}
