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
    const auto dataMap = variantMap["Data"].toMap();

    variantMapMustContain(dataMap, "Clusters");
    variantMapMustContain(dataMap, "IndicesRawData");
    variantMapMustContain(dataMap, "NumberOfIndices");

    // Packed indices for all clusters
    QVector<std::uint32_t> packedIndices;

    packedIndices.resize(dataMap["NumberOfIndices"].toInt());

    // Convert raw data to indices
    populateDataBufferFromVariantMap(dataMap["IndicesRawData"].toMap(), (char*)packedIndices.data());

    // Get list of clusters
    const auto clustersList = dataMap["Clusters"].toList();

    _clusters.resize(clustersList.count());

    // Populate clusters
    for (const auto& clusterVariant : clustersList) {

        // Get cluster parameters and index
        const auto clusterMap       = clusterVariant.toMap();
        const auto clusterIndex     = clustersList.indexOf(clusterMap);

        // Get reference to current cluster
        auto& cluster = _clusters[clusterIndex];

        cluster.setName(clusterMap["Name"].toString());
        cluster.setId(clusterMap["ID"].toString());
        cluster.setColor(clusterMap["Color"].toString());

        // Get the offset into the packed indices vector and the number of indices in the cluster
        const auto globalIndicesOffset  = clusterMap["GlobalIndicesOffset"].toInt();
        const auto numberOfIndices      = clusterMap["NumberOfIndices"].toInt();

        // Copy packed indices to cluster indices
        cluster.getIndices() = std::vector<std::uint32_t>(packedIndices.begin() + globalIndicesOffset, packedIndices.begin() + globalIndicesOffset + numberOfIndices);
    }
}

QVariantMap ClusterData::toVariantMap() const
{
    QVariantList clusters;

    // All cluster indices
    std::vector<std::uint32_t> indices;

    // Build vector that includes all cluster indices
    for (const auto& cluster : _clusters)
        indices.insert(indices.end(), cluster.getIndices().begin(), cluster.getIndices().end());

    QVariantMap indicesRawData = rawDataToVariantMap((char*)indices.data(), indices.size() * sizeof(std::uint32_t), true);

    std::size_t globalIndicesOffset = 0;

    // Create list of clusters
    for (const auto& cluster : _clusters) {

        // Get the number of indices in the cluster
        const auto numberOfIndicesInCluster = cluster.getIndices().size();

        // Add variant map for each cluster
        clusters.append(QVariantMap({
            { "Name", cluster.getName() },
            { "ID", cluster.getId() },
            { "Color", cluster.getColor() },
            { "GlobalIndicesOffset", QVariant::fromValue(globalIndicesOffset) },
            { "NumberOfIndices", QVariant::fromValue(numberOfIndicesInCluster) }
        }));

        // Compute global indices offset
        globalIndicesOffset += numberOfIndicesInCluster;
    }

    return {
        { "Clusters", clusters },
        { "IndicesRawData", indicesRawData },
        { "NumberOfIndices", QVariant::fromValue(indices.size()) }
    };
}

void Clusters::init()
{
    _infoAction = QSharedPointer<InfoAction>::create(nullptr, *this);

    addAction(*_infoAction.get());

    _eventListener.setEventCore(_core);
    _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DataSelectionChanged));
    _eventListener.registerDataEventByType(ClusterType, [this](DataEvent* dataEvent) {

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

    _core->notifyDatasetChanged(this);
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
    _core->notifyDatasetSelectionChanged(this);

    // Get reference to input dataset
    auto points             = getDataHierarchyItem().getParent().getDataset<Points>();
    auto selection          = points->getSelection<Points>();
    auto selectionIndices   = selection->indices;

    selectionIndices.clear();
    selectionIndices.reserve(indices.size());

    std::vector<std::uint32_t> globalIndices;

    points->getGlobalIndices(globalIndices);

    // Append point indices per cluster
    for (auto clusterSelectionIndex : getSelection<Clusters>()->indices) {
        const auto cluster = getClusters().at(clusterSelectionIndex);
        selectionIndices.insert(selectionIndices.end(), cluster.getIndices().begin(), cluster.getIndices().end());
    }

    // Remove duplicates
    std::sort(selectionIndices.begin(), selectionIndices.end());
    selectionIndices.erase(unique(selectionIndices.begin(), selectionIndices.end()), selectionIndices.end());

    for (auto& pointSelectionIndex : selectionIndices)
        pointSelectionIndex = globalIndices[pointSelectionIndex];

    points->setSelectionIndices(selectionIndices);

    _core->notifyDatasetSelectionChanged(points);
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
    _core->notifyDatasetSelectionChanged(this);
}

void Clusters::selectNone()
{
    // Clear selection indices
    getSelectionIndices().clear();

    // Notify others that the selection changed
    _core->notifyDatasetSelectionChanged(this);
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
    _core->notifyDatasetSelectionChanged(this);
}

QIcon ClusterDataFactory::getIcon() const
{
    return Application::getIconFont("FontAwesome").getIcon("th-large");
}

hdps::plugin::RawData* ClusterDataFactory::produce()
{
    return new ClusterData(this);
}
