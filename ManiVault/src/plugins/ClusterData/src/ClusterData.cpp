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

namespace {

    template <typename T>
    concept RawStreamable = std::is_arithmetic_v<T> && std::is_trivially_copyable_v<T>;

    constexpr quint32 kClusterStreamVersion = 1;

    template <RawStreamable T>
    QDataStream& writeRawVector(QDataStream& out, const std::vector<T>& values)
    {
        if (values.size() > std::numeric_limits<quint32>::max()) {
            out.setStatus(QDataStream::WriteFailed);
            return out;
        }

        const auto size = static_cast<quint32>(values.size());
        out << size;

        if (size == 0)
            return out;

        const qsizetype bytes =
            qsizetype(values.size()) * qsizetype(sizeof(T));

        const auto written = out.writeRawData(
            reinterpret_cast<const char*>(values.data()),
            bytes
        );

        if (written != bytes)
            out.setStatus(QDataStream::WriteFailed);

        return out;
    }

    template <RawStreamable T>
    QDataStream& readRawVector(QDataStream& in, std::vector<T>& values)
    {
        quint32 size = 0;
        in >> size;

        if (in.status() != QDataStream::Ok)
            return in;

        const qsizetype bytes =
            qsizetype(size) * qsizetype(sizeof(T));

        if (size != 0 && bytes / qsizetype(sizeof(T)) != qsizetype(size)) {
            in.setStatus(QDataStream::ReadCorruptData);
            return in;
        }

        values.resize(size);

        if (size == 0)
            return in;

        const auto read = in.readRawData(
            reinterpret_cast<char*>(values.data()),
            bytes
        );

        if (read != bytes)
            in.setStatus(QDataStream::ReadPastEnd);

        return in;
    }

}

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

//struct ClustersLoadContext : public WorkflowContextBase
//{
//    explicit ClustersLoadContext(const QVariantMap& rawDataMap) :
//		_rawDataMap(rawDataMap)
//    {
//    }
//
//    QVariantMap _rawDataMap;
//    QByteArray  _decodedBytes;
//    QVector<Cluster> _loadedClusters;
//};

//QDataStream& operator>>(QDataStream& in, QVector<Cluster>& clusters)
//{
//    quint32 size = 0;
//    in >> size;
//
//    if (in.status() != QDataStream::Ok)
//        return in;
//
//    clusters.clear();
//    clusters.reserve(size);
//
//    for (quint32 i = 0; i < size; ++i) {
//        Cluster cluster;
//        in >> cluster;
//
//        if (in.status() != QDataStream::Ok)
//            return in;
//
//        clusters.emplace_back(std::move(cluster));
//    }
//
//    return in;
//}
//
//QDataStream& operator<<(QDataStream& out, const QVector<Cluster>& clusters)
//{
//    if (clusters.size() > std::numeric_limits<quint32>::max()) {
//        out.setStatus(QDataStream::WriteFailed);
//        return out;
//    }
//
//    out << static_cast<quint32>(clusters.size());
//
//    for (const auto& cluster : clusters)
//        out << cluster;
//
//    return out;
//}

struct SerializedClusterHeader
{
    QString name;
    QString id;
    QColor color;

    quint64 indexOffset = 0;
    quint64 indexCount = 0;

    std::vector<float> median;
    std::vector<float> mean;
    std::vector<float> stddev;
};

void ClusterData::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    const auto dataMap                      = variantMap["Data"].toMap();
    const auto projectApplicationVersion    = mv::projects().getCurrentProject()->getApplicationVersionAction().getVersion();

    if (projectApplicationVersion < Version(1, 5, 0)) {
        fromVariantMapPre150(variantMap);
    } else {
        /*
        QVector<Cluster> clusters;

        QByteArray headersBytes =
            populateDataBufferFromVariantMapSync(
                dataMap["ClustersMetaData"].toMap()
            );

        QDataStream headersStream(&headersBytes, QIODevice::ReadOnly);
        headersStream.setVersion(QDataStream::Qt_6_5);

        std::uint64_t clusterCount = 0;
        headersStream >> clusterCount;

        if (headersStream.status() != QDataStream::Ok)
            throw std::runtime_error("Failed to read cluster metadata count");

        std::vector<SerializedClusterHeader> headers;
        headers.reserve(clusterCount);

        for (std::uint64_t i = 0; i < clusterCount; ++i) {
            SerializedClusterHeader header;

            headersStream
                >> header.name
                >> header.id
                >> header.color
                >> header.indexOffset
                >> header.indexCount;

            readRawVector(headersStream, header.median);
            readRawVector(headersStream, header.mean);
            readRawVector(headersStream, header.stddev);

            if (headersStream.status() != QDataStream::Ok)
                throw std::runtime_error("Failed to deserialize cluster header");

            headers.emplace_back(std::move(header));
        }

        //
        // Load contiguous indices blob
        //
        */

        QByteArray indicesBytes =
            populateDataBufferFromVariantMapSync(
                dataMap["ClustersIndicesRawData"].toMap()
            );

        if (indicesBytes.size() % qsizetype(sizeof(std::uint32_t)) != 0)
            throw std::runtime_error("Cluster indices blob size is invalid");

        std::vector<std::uint32_t> allIndices(
            indicesBytes.size() / sizeof(std::uint32_t)
        );

        std::memcpy(
            allIndices.data(),
            indicesBytes.constData(),
            size_t(indicesBytes.size())
        );

        //
        // Reconstruct clusters
        //
        /*
        clusters.reserve(headers.size());

        for (const auto& header : headers) {
            Cluster cluster;

            cluster.setName(header.name);
            cluster.setId(header.id);
            cluster.setColor(header.color);

            //cluster.setMedian(header.median);
            //cluster.setMean(header.mean);
            //cluster.setStandardDeviation(header.stddev);

            const auto beginOffset =
                static_cast<std::size_t>(header.indexOffset);

            const auto count =
                static_cast<std::size_t>(header.indexCount);

            if (beginOffset + count > allIndices.size())
                throw std::runtime_error("Cluster index range exceeds index buffer");

            auto& indices = cluster.getIndices();

            indices.assign(
                allIndices.begin() + beginOffset,
                allIndices.begin() + beginOffset + count
            );

            clusters.emplace_back(std::move(cluster));
        }

        _clusters = std::move(clusters);
        */
        //populateDataBufferFromVariantMapAsync(dataMap["ClustersRawData"].toMap(), this, [this](const SharedDataBuffer& data) {
        //    QDataStream clustersDataStream(data.data(), QIODevice::ReadOnly);
        //    clustersDataStream.setVersion(QDataStream::Qt_6_5);

        //    QVector<Cluster> clusters;
        //    clustersDataStream >> clusters;

        //    if (clustersDataStream.status() != QDataStream::Ok) {
        //        qCritical() << "Failed to deserialize cluster payload";
        //        return;
        //    }

        //    _clusters = std::move(clusters);
        //});
    }
}

void ClusterData::fromVariantMapPre150(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    const auto dataMap = variantMap["Data"].toMap();

    variantMapMustContain(dataMap, "IndicesRawData");
    variantMapMustContain(dataMap, "NumberOfIndices");

    // Packed indices for all clusters
    QVector<std::uint32_t> packedIndices;

    packedIndices.resize(dataMap["NumberOfIndices"].toInt());

    
    
    // Convert raw data to indices
    populateDataBufferFromVariantMapToRawBufferSync(dataMap["IndicesRawData"].toMap(), (char*)packedIndices.data(), packedIndices.size() * sizeof(std::uint32_t));

    if (dataMap.contains("ClustersRawData")) {
        QByteArray clustersByteArray;

        QDataStream clustersDataStream(&clustersByteArray, QIODevice::ReadOnly);

        const auto clustersRawDataSize = dataMap["ClustersRawDataSize"].toInt();

        clustersByteArray.resize(clustersRawDataSize);
        
        qDebug() << "---ClusterData::fromVariantMapPre150---ClustersRawData";
        populateDataBufferFromVariantMapToRawBufferSync(dataMap["ClustersRawData"].toMap(), (char*)clustersByteArray.data(), clustersByteArray.size());

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

            const auto globalIndicesOffset = clusterMap["GlobalIndicesOffset"].toInt();
            const auto numberOfIndices = clusterMap["NumberOfIndices"].toInt();

            cluster.getIndices() = std::vector<std::uint32_t>(packedIndices.begin() + globalIndicesOffset, packedIndices.begin() + globalIndicesOffset + numberOfIndices);

            ++clusterIndex;
        }
    }

    // For backwards compatibility
    if (dataMap.contains("Clusters")) {
        const auto clustersList = dataMap["Clusters"].toList();

        _clusters.resize(clustersList.count());

        for (const auto& clusterVariant : clustersList) {
            const auto clusterMap = clusterVariant.toMap();
            const auto clusterIndex = clustersList.indexOf(clusterMap);

            auto& cluster = _clusters[clusterIndex];

            cluster.setName(clusterMap["Name"].toString());
            cluster.setId(clusterMap["ID"].toString());
            cluster.setColor(clusterMap["Color"].toString());

            const auto globalIndicesOffset = clusterMap["GlobalIndicesOffset"].toInt();
            const auto numberOfIndices = clusterMap["NumberOfIndices"].toInt();

            cluster.getIndices() = std::vector<std::uint32_t>(packedIndices.begin() + globalIndicesOffset, packedIndices.begin() + globalIndicesOffset + numberOfIndices);
        }
    }
}



QVariantMap ClusterData::toVariantMap() const
{
    auto variantMap = RawData::toVariantMap();

    //QByteArray clustersByteArray;
    //QDataStream clustersDataStream(&clustersByteArray, QIODevice::WriteOnly);

    //clustersDataStream.setVersion(QDataStream::Qt_6_5);

    //clustersDataStream << _clusters;

    //variantMap["ClustersRawDataSize"]   = clustersByteArray.size();
    //variantMap["ClustersRawData"]       = rawDataToVariantMap(clustersByteArray.constData(), clustersByteArray.size(), nullptr);

    std::vector<SerializedClusterHeader> headers;
    std::vector<unsigned int> allIndices;

    headers.reserve(_clusters.size());

    QByteArray headersByteArray;
    QDataStream headersStream(&headersByteArray, QIODevice::WriteOnly);
    headersStream.setVersion(QDataStream::Qt_6_5);

    headersStream << static_cast<quint32>(headers.size());

    std::uint64_t indexOffset = 0;

    for (const auto& cluster : _clusters) {
        const auto indexCount = cluster.getIndices().size();

        headersStream
            << cluster.getName()
            << cluster.getId()
            << cluster.getColor()
            << static_cast<quint64>(indexOffset)
            << static_cast<quint64>(indexCount);

        writeRawVector(headersStream, cluster.getMedian());
        writeRawVector(headersStream, cluster.getMean());
        writeRawVector(headersStream, cluster.getStandardDeviation());

        indexOffset += indexCount;

        allIndices.insert(allIndices.end(), cluster.getIndices().begin(), cluster.getIndices().end());
    }

    if (headersStream.status() != QDataStream::Ok)
        throw std::runtime_error("Failed to serialize cluster metadata");

    variantMap["ClustersFormatVersion"] = 2;

    variantMap["ClustersMetaDataSize"] = headersByteArray.size();
    variantMap["ClustersMetaData"] = rawDataToVariantMap(
        headersByteArray.constData(),
        headersByteArray.size(),
        nullptr
    );

    const auto indicesByteSize =
        allIndices.size() * sizeof(std::uint32_t);

    variantMap["ClustersIndicesRawDataSize"] = static_cast<qulonglong>(indicesByteSize);
    variantMap["ClustersIndicesRawData"] = rawDataToVariantMap(
        reinterpret_cast<const char*>(allIndices.data()),
        indicesByteSize,
        nullptr
    );

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

    setIconByName("table-cells-large");
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
    const auto projectApplicationVersion = mv::projects().getCurrentProject()->getApplicationVersionAction().getVersion();

    if (projectApplicationVersion < Version(1, 5, 0)) {
        fromVariantMapPre150(variantMap);
    }
    else {
        DatasetImpl::fromVariantMap(variantMap);

        getRawData<ClusterData>()->fromVariantMap(variantMap);
    }
}

void Clusters::fromVariantMapPre150(const QVariantMap& variantMap)
{
    DatasetImpl::fromVariantMapPre150(variantMap);

    getRawData<ClusterData>()->fromVariantMapPre150(variantMap);

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
    setIconByName("table-cells-large");
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
