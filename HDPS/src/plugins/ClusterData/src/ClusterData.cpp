#include "ClusterData.h"
#include "InfoAction.h"

#include "Dataset.h"
#include "DataHierarchyItem.h"
#include "event/Event.h"
#include "PointData.h"

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

Dataset<DatasetImpl> ClusterData::createDataSet() const
{
    return Dataset<DatasetImpl>(new Clusters(_core, getName()));
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

std::int32_t ClusterData::getClusterIndex(const QString& clusterName) const
{
    std::int32_t clusterIndex = 0;

    // Loop over all clusters and see if the name matches
    for (const auto& cluster : _clusters){

        if (clusterName == cluster.getName())
            return clusterIndex;

        clusterIndex++;
    }

    return -1;
}

void Clusters::init()
{
    _infoAction = QSharedPointer<InfoAction>::create(nullptr, *this);

    addAction(*_infoAction.get());

    setEventCore(_core);

    registerDataEventByType(ClusterType, [this](DataEvent* dataEvent) {
        if (dataEvent->getDataset() == Dataset<Clusters>(this))
            return;

        if (getGroupIndex() < 0)
            return;

        if (dataEvent->getDataset()->getGroupIndex() == getGroupIndex() && dataEvent->getDataset()->getDataType() == ClusterType) {

            // Get smart pointer to foreign clusters dataset
            auto foreignClusters = dataEvent->getDataset<Clusters>();

            // Get names of the selected clusters in the foreign dataset
            const auto foreignSelectedClusterNames = foreignClusters->getSelectedClusterNames();

            // Attempt to select clusters (fails if the clusters are the same)
            setSelection(foreignSelectedClusterNames);
        }
    });
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
    for (const auto& clusterIndex : getSelection<Clusters>()->indices) {

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

QStringList Clusters::getSelectedClusterNames() const
{
    QStringList clusterNames;

    for (const auto& selectedIndex : getSelection<Clusters>()->indices)
        clusterNames  << getClusters()[selectedIndex].getName();

    return clusterNames;
}

void Clusters::setSelection(const QStringList& clusterNames)
{
    // Exit if nothing changed
    if (clusterNames == getSelectedClusterNames())
        return;

    // New selection indices
    std::vector<std::uint32_t> selectionIndices;

    for (const auto& clusterName : clusterNames) {

        // Get cluster index
        const auto clusterIndex = getRawData<ClusterData>().getClusterIndex(clusterName);

        if (clusterIndex >= 0)
            selectionIndices.push_back(clusterIndex);
    }

    // Set the selection
    setSelection(selectionIndices);
}

void Clusters::setSelection(const std::vector<std::uint32_t>& indices)
{
    // No need to process equal selections
    if (indices == getSelection<Clusters>()->indices)
        return;

    // Assign new selection
    getSelection<Clusters>()->indices = indices;

    // Notify others that the cluster selection has changed
    _core->notifyDataSelectionChanged(this);

    // Get reference to input dataset
    auto& parentDataset = getDataHierarchyItem().getParent().getDataset<DatasetImpl>();

    // Select points
    if (parentDataset->getDataType() == PointType) {

        auto& points                = Dataset<Points>(parentDataset);
        auto& selection             = points->getSelection<Points>();
        auto& pointSelectionIndices = selection->indices;

        pointSelectionIndices.clear();
        pointSelectionIndices.reserve(indices.size());

        std::vector<std::uint32_t> globalIndices;

        points->getGlobalIndices(globalIndices);

        // Append point indices per cluster
        for (auto clusterSelectionIndex : getSelection<Clusters>()->indices) {
            
            // Get cluster
            const auto cluster = getClusters().at(clusterSelectionIndex);

            // Append the indices
            pointSelectionIndices.insert(pointSelectionIndices.end(), cluster.getIndices().begin(), cluster.getIndices().end());
        }

        // Remove duplicates
        std::sort(pointSelectionIndices.begin(), pointSelectionIndices.end());
        pointSelectionIndices.erase(unique(pointSelectionIndices.begin(), pointSelectionIndices.end()), pointSelectionIndices.end());

        // Notify others that the parent points selection has changed
        _core->notifyDataSelectionChanged(parentDataset);
    }
}

QIcon ClusterDataFactory::getIcon() const
{
    return Application::getIconFont("FontAwesome").getIcon("th-large");
}

hdps::plugin::RawData* ClusterDataFactory::produce()
{
    return new ClusterData(this);
}
