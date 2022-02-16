#include "ClusterData.h"
#include "InfoAction.h"

#include "Dataset.h"
#include "DataHierarchyItem.h"
#include "event/Event.h"
#include "PointData.h"

#include "Application.h"

#include <util/Serialization.h>

#include <QtCore>
#include <QtDebug>

#include <set>

Q_PLUGIN_METADATA(IID "hdps.ClusterData")

using namespace hdps::util;

ClusterData::ClusterData(const hdps::plugin::PluginFactory* factory) :
    hdps::plugin::RawData(factory, ClusterType)
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

QVector<Cluster>& ClusterData::getClusters()
{
    return _clusters;
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

void ClusterData::fromVariantMap(const QVariantMap& variantMap)
{
    variantMapMustContain(variantMap, "Data");

    const auto data = variantMap["Data"].toMap();

    variantMapMustContain(data, "Clusters");

    const auto clustersList = data["Clusters"].toList();

    _clusters.resize(clustersList.count());

    for (const auto& clusterVariant : clustersList) {

        const auto clusterMap       = clusterVariant.toMap();
        const auto clusterIndex     = clustersList.indexOf(clusterMap);

        auto& cluster = _clusters[clusterIndex];

        cluster.setName(clusterMap["Name"].toString());
        cluster.setId(clusterMap["ID"].toString());
        cluster.setColor(clusterMap["Color"].toString());

        const auto& indicesMap = clusterMap["Indices"].toMap();

        cluster.getIndices().resize(indicesMap["Count"].toInt());

        populateDataBufferFromVariantMap(indicesMap["Raw"].toMap(), (char*)cluster.getIndices().data());
    }
}

QVariantMap ClusterData::toVariantMap() const
{
    QVariantList clusters;

    for (const auto& cluster : _clusters) {

        QVariantMap indicesMap;

        indicesMap["Count"] = cluster.getIndices().size();
        indicesMap["Raw"]   = rawDataToVariantMap((char*)cluster.getIndices().data(), cluster.getIndices().size() * sizeof(std::uint32_t));

        clusters.append(QVariantMap({
            { "Name", cluster.getName() },
            { "ID", cluster.getId() },
            { "Color", cluster.getColor() },
            { "Indices", indicesMap }
        }));
    }

    return {
        { "Clusters", clusters }
    };
}

void Clusters::init()
{
    _infoAction = QSharedPointer<InfoAction>::create(nullptr, *this);

    addAction(*_infoAction.get());

    setEventCore(_core);

    registerDataEventByType(ClusterType, [this](DataEvent* dataEvent) {

        // Only process selection changes
        if (dataEvent->getType() != EventType::DataSelectionChanged)
            return;

        // Do not process our own selection changes
        if (dataEvent->getDataset() == Dataset<Clusters>(this))
            return;

        // Only synchronize when our own group index is non-negative
        if (!_core->isDatasetGroupingEnabled() || getGroupIndex() < 0)
            return;

        if (dataEvent->getDataset()->getGroupIndex() == getGroupIndex() && dataEvent->getDataset()->getDataType() == ClusterType) {

            // Get smart pointer to foreign clusters dataset
            auto foreignClusters = dataEvent->getDataset<Clusters>();

            // Get names of the selected clusters in the foreign dataset
            const auto foreignSelectedClusterNames = foreignClusters->getSelectionNames();

            // Attempt to select clusters (fails if the clusters are the same)
            setSelectionNames(foreignSelectedClusterNames);
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

void Clusters::fromVariantMap(const QVariantMap& variantMap)
{
    DatasetImpl::fromVariantMap(variantMap);

    getRawData<ClusterData>().fromVariantMap(variantMap);

    _core->notifyDataChanged(this);
}

QVariantMap Clusters::toVariantMap() const
{
    auto variantMap = DatasetImpl::toVariantMap();

    variantMap["Data"] = getRawData<ClusterData>().toVariantMap();

    return variantMap;
}

std::vector<std::uint32_t>& Clusters::getSelectionIndices()
{
    return getSelection<Clusters>()->indices;
}

void Clusters::setSelectionIndices(const std::vector<std::uint32_t>& indices)
{
    // Assign new selection
    getSelection<Clusters>()->indices = indices;

    // Notify others that the cluster selection has changed
    _core->notifyDataSelectionChanged(this);

    // Get reference to input dataset
    auto points                 = getDataHierarchyItem().getParent().getDataset<Points>();
    auto selection              = points->getSelection<Points>();
    auto& pointSelectionIndices = selection->indices;

    pointSelectionIndices.clear();
    pointSelectionIndices.reserve(indices.size());

    std::vector<std::uint32_t> globalIndices;

    points->getGlobalIndices(globalIndices);

    // Append point indices per cluster
    for (auto clusterSelectionIndex : getSelection<Clusters>()->indices) {

        // Get cluster point indices and append
        const auto cluster = getClusters().at(clusterSelectionIndex);

        pointSelectionIndices.insert(pointSelectionIndices.end(), cluster.getIndices().begin(), cluster.getIndices().end());
    }

    // Remove duplicates
    std::sort(pointSelectionIndices.begin(), pointSelectionIndices.end());
    pointSelectionIndices.erase(unique(pointSelectionIndices.begin(), pointSelectionIndices.end()), pointSelectionIndices.end());

    for (auto& pointSelectionIndex : pointSelectionIndices)
        pointSelectionIndex = globalIndices[pointSelectionIndex];

    // Notify others that the parent points selection has changed
    _core->notifyDataSelectionChanged(points);
}

QStringList Clusters::getSelectionNames() const
{
    QStringList clusterNames;

    for (const auto& selectedIndex : getSelection<Clusters>()->indices)
        clusterNames << getClusters()[selectedIndex].getName();

    return clusterNames;
}

void Clusters::setSelectionNames(const QStringList& clusterNames)
{
    // Exit if nothing changed
    if (clusterNames == getSelectionNames())
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
    setSelectionIndices(selectionIndices);
}

bool Clusters::canSelect() const
{
    return getClusters().size() >= 1;
}

bool Clusters::canSelectAll() const
{
    return getSelectionSize() < getClusters().size();
}

bool Clusters::canSelectNone() const
{
    return getSelectionSize() >= 1;
}

bool Clusters::canSelectInvert() const
{
    return true;
}

void Clusters::selectAll()
{
    // Get reference to selection indices
    auto& selectionIndices = getSelectionIndices();

    // Clear and resize
    selectionIndices.clear();
    selectionIndices.resize(getClusters().size());

    // Generate cluster selection indices
    std::iota(selectionIndices.begin(), selectionIndices.end(), 0);

    // Notify others that the selection changed
    _core->notifyDataSelectionChanged(this);
}

void Clusters::selectNone()
{
    // Clear selection indices
    getSelectionIndices().clear();

    // Notify others that the selection changed
    _core->notifyDataSelectionChanged(this);
}

void Clusters::selectInvert()
{
    // Get reference to selection indices
    auto& selectionIndices = getSelectionIndices();

    // Create set of selected indices
    std::set<std::uint32_t> selectionSet(selectionIndices.begin(), selectionIndices.end());

    // Get number of clusters
    const auto numberOfClusters = getClusters().size();

    // Clear and resize
    selectionIndices.clear();
    selectionIndices.reserve(numberOfClusters - selectionSet.size());

    // Do the inversion
    for (std::uint32_t i = 0; i < static_cast<std::uint32_t>(numberOfClusters); i++) {
        if (selectionSet.find(i) == selectionSet.end())
            selectionIndices.push_back(i);
    }

    // Notify others that the selection changed
    _core->notifyDataSelectionChanged(this);
}

QIcon ClusterDataFactory::getIcon() const
{
    return Application::getIconFont("FontAwesome").getIcon("th-large");
}

hdps::plugin::RawData* ClusterDataFactory::produce()
{
    return new ClusterData(this);
}
