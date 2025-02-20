// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ClusterData.h"
#include "InfoAction.h"

#include "Dataset.h"
#include "DataHierarchyItem.h"
#include "event/Event.h"
#include "PointData/PointData.h"

#include "Application.h"

#include <util/Serialization.h>

#include <QtCore>
#include <QtDebug>

#include <set>

Q_PLUGIN_METADATA(IID "studio.manivault.ClusterData")

using namespace mv::util;

ClusterData::ClusterData(const mv::plugin::PluginFactory* factory) :
    mv::plugin::RawData(factory, ClusterType)
{
}

ClusterData::~ClusterData(void)
{
}

void ClusterData::init()
{
}

Dataset<DatasetImpl> ClusterData::createDataSet(const QString& guid /*= ""*/) const
{
    return Dataset<DatasetImpl>(new Clusters(getName(), false, guid));
}

QVector<Cluster>& ClusterData::getClusters()
{
    return _clusters;
}

const QVector<Cluster>& ClusterData::getClusters() const
{
    return _clusters;
}

void ClusterData::setClusters(const QVector<Cluster>&clusters)
{
    _clusters = clusters;
}

void ClusterData::addCluster(Cluster& cluster)
{
    _clusters.push_back(cluster);
}

void ClusterData::setClusterNames(const std::vector<QString>& clusterNames)
{
    if (clusterNames.empty())
        return;

    if (clusterNames.size() != _clusters.size())
    {
        qWarning() << "ClusterData: Number of cluster names does not equal the number of data cluster. No cluster names assigned.";
        return;
    }

    for (size_t i = 0; i < clusterNames.size(); i++)
        _clusters[i].setName(clusterNames[i]);
}

std::vector<QString> ClusterData::getClusterNames()
{
    std::vector<QString> clusterNames;
    clusterNames.reserve(_clusters.size());

    for (const auto& clusters : _clusters)
        clusterNames.push_back(clusters.getName());

    return clusterNames;
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
    WidgetAction::fromVariantMap(variantMap);

    const auto dataMap = variantMap["Data"].toMap();

    variantMapMustContain(dataMap, "IndicesRawData");
    variantMapMustContain(dataMap, "NumberOfIndices");

    // Packed indices for all clusters
    QVector<std::uint32_t> packedIndices;

    packedIndices.resize(dataMap["NumberOfIndices"].toInt());

    // Convert raw data to indices
    populateDataBufferFromVariantMap(dataMap["IndicesRawData"].toMap(), (char*)packedIndices.data());

    if (dataMap.contains("ClustersRawData")) {
        QByteArray clustersByteArray;

        QDataStream clustersDataStream(&clustersByteArray, QIODevice::ReadOnly);

        const auto clustersRawDataSize = dataMap["ClustersRawDataSize"].toInt();

        clustersByteArray.resize(clustersRawDataSize);

        populateDataBufferFromVariantMap(dataMap["ClustersRawData"].toMap(), (char*)clustersByteArray.data());

        QVariantList clusters;

        clustersDataStream >> clusters;

        _clusters.resize(clusters.count());

        long clusterIndex = 0;

        for (const auto& clusterVariant : clusters) {
            const auto clusterMap = clusterVariant.toMap();

            auto& cluster = _clusters[clusterIndex];

            cluster.setName(clusterMap["Name"].toString());
            cluster.setId(clusterMap["ID"].toString());
            cluster.setColor(clusterMap["Color"].toString());

            const auto globalIndicesOffset  = clusterMap["GlobalIndicesOffset"].toInt();
            const auto numberOfIndices      = clusterMap["NumberOfIndices"].toInt();

            cluster.getIndices() = std::vector<std::uint32_t>(packedIndices.begin() + globalIndicesOffset, packedIndices.begin() + globalIndicesOffset + numberOfIndices);

            ++clusterIndex;
        }
    }
    
    // For backwards compatibility
    if (dataMap.contains("Clusters")) {
        const auto clustersList = dataMap["Clusters"].toList();

        _clusters.resize(clustersList.count());

        for (const auto& clusterVariant : clustersList) {
            const auto clusterMap   = clusterVariant.toMap();
            const auto clusterIndex = clustersList.indexOf(clusterMap);

            auto& cluster = _clusters[clusterIndex];

            cluster.setName(clusterMap["Name"].toString());
            cluster.setId(clusterMap["ID"].toString());
            cluster.setColor(clusterMap["Color"].toString());

            const auto globalIndicesOffset  = clusterMap["GlobalIndicesOffset"].toInt();
            const auto numberOfIndices      = clusterMap["NumberOfIndices"].toInt();

            cluster.getIndices() = std::vector<std::uint32_t>(packedIndices.begin() + globalIndicesOffset, packedIndices.begin() + globalIndicesOffset + numberOfIndices);
        }
    }
}

QVariantMap ClusterData::toVariantMap() const
{
    auto variantMap = WidgetAction::toVariantMap();

    std::vector<std::uint32_t> indices;

    for (const auto& cluster : _clusters)
        indices.insert(indices.end(), cluster.getIndices().begin(), cluster.getIndices().end());

    QVariantMap indicesRawData = rawDataToVariantMap((char*)indices.data(), indices.size() * sizeof(std::uint32_t), true);

    std::size_t globalIndicesOffset = 0;

    QVariantList clusters;

    clusters.reserve(_clusters.count());

    for (const auto& cluster : _clusters) {
        const auto numberOfIndicesInCluster = cluster.getIndices().size();

        clusters.push_back(QVariantMap({
            { "Name", cluster.getName() },
            { "ID", cluster.getId() },
            { "Color", cluster.getColor() },
            { "GlobalIndicesOffset", QVariant::fromValue(globalIndicesOffset) },
            { "NumberOfIndices", QVariant::fromValue(numberOfIndicesInCluster) }
        }));

        globalIndicesOffset += numberOfIndicesInCluster;
    }

    // https://stackoverflow.com/questions/19537186/serializing-qvariant-through-qdatastream

    QByteArray clustersByteArray;
    QDataStream clustersDataStream(&clustersByteArray, QIODevice::WriteOnly);

    clustersDataStream << clusters;

    QVariantMap clustersRawData = rawDataToVariantMap((char*)clustersByteArray.data(), clustersByteArray.size(), true);

    variantMap.insert({
        { "ClustersRawData", clustersRawData },
        { "ClustersRawDataSize", clustersByteArray.size() },
        { "IndicesRawData", indicesRawData },
        { "NumberOfIndices", QVariant::fromValue(indices.size()) }
    });

    return variantMap;
}

void Clusters::init()
{
    _infoAction = QSharedPointer<InfoAction>::create(nullptr, *this);

    addAction(*_infoAction.get());

    _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DatasetDataSelectionChanged));
    _eventListener.registerDataEventByType(ClusterType, [this](DatasetEvent* dataEvent) {

        // Only process selection changes
        if (dataEvent->getType() != EventType::DatasetDataSelectionChanged)
            return;

        // Do not process our own selection changes
        if (dataEvent->getDataset() == Dataset<Clusters>(this))
            return;

        // Only synchronize when our own group index is non-negative
        if (!mv::data().getSelectionGroupingAction()->isChecked() || getGroupIndex() < 0)
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
    getRawData<ClusterData>()->addCluster(cluster);
}

void Clusters::removeClusterById(const QString& id)
{
    getRawData<ClusterData>()->removeClusterById(id);
}

void Clusters::removeClustersById(const QStringList& ids)
{
    getRawData<ClusterData>()->removeClustersById(ids);
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

    getRawData<ClusterData>()->fromVariantMap(variantMap);

    events().notifyDatasetDataChanged(this);
}

QVariantMap Clusters::toVariantMap() const
{
    auto variantMap = DatasetImpl::toVariantMap();

    variantMap["Data"] = getRawData<ClusterData>()->toVariantMap();

    return variantMap;
}

std::vector<std::uint32_t>& Clusters::getSelectionIndices()
{
    return getSelection<Clusters>()->indices;
}

void Clusters::setSelectionIndices(const std::vector<std::uint32_t>& indices)
{
    getSelection<Clusters>()->indices = indices;

    events().notifyDatasetDataSelectionChanged(this);

    if (!getDataHierarchyItem().getParent())
        return;

    // Get reference to input dataset
    auto points             = getDataHierarchyItem().getParent()->getDataset<Points>();
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

    points->setSelectionIndices(selectionIndices);

    events().notifyDatasetDataSelectionChanged(points);
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
        const auto clusterIndex = getRawData<ClusterData>()->getClusterIndex(clusterName);

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
    auto& selectionIndices = getSelectionIndices();

    selectionIndices.clear();
    selectionIndices.resize(getClusters().size());

    std::iota(selectionIndices.begin(), selectionIndices.end(), 0);

    events().notifyDatasetDataSelectionChanged(this);
}

void Clusters::selectNone()
{
    getSelectionIndices().clear();

    events().notifyDatasetDataSelectionChanged(this);
}

void Clusters::selectInvert()
{
    auto& selectionIndices = getSelectionIndices();

    std::set<std::uint32_t> selectionSet(selectionIndices.begin(), selectionIndices.end());

    const auto numberOfClusters = getClusters().size();

    selectionIndices.clear();
    selectionIndices.reserve(numberOfClusters - selectionSet.size());

    for (std::uint32_t i = 0; i < static_cast<std::uint32_t>(numberOfClusters); i++) {
        if (selectionSet.find(i) == selectionSet.end())
            selectionIndices.push_back(i);
    }

    events().notifyDatasetDataSelectionChanged(this);
}

ClusterDataFactory::ClusterDataFactory()
{
    setIconByName("th-large");
}

QUrl ClusterDataFactory::getReadmeMarkdownUrl() const
{
#ifdef ON_LEARNING_CENTER_FEATURE_BRANCH
    return QUrl("https://raw.githubusercontent.com/ManiVaultStudio/core/feature/learning_center/ManiVault/src/plugins/ClusterData/README.md");
#else
    return QUrl("https://raw.githubusercontent.com/ManiVaultStudio/core/master/ManiVault/src/plugins/ClusterData/README.md");
#endif
}

QUrl ClusterDataFactory::getRepositoryUrl() const
{
    return QUrl("https://github.com/ManiVaultStudio/core");
}

mv::plugin::RawData* ClusterDataFactory::produce()
{
    return new ClusterData(this);
}
