// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "ClustersSerializer.h"

#include <CoreInterface.h>
#include <util/Serialization.h>

#include <QDataStream>

#include <algorithm>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <numeric>

using namespace mv::util;

namespace
{
    enum class ClusterHeaderStorageMode : quint32
    {
        General = 0,
        Singleton = 1
    };

#pragma pack(push, 1)
    struct RawClusterHeader
    {
        quint32 nameOffset = 0;
        quint32 nameSize = 0;
        quint32 idOffset = 0;
        quint32 idSize = 0;
        quint32 rgba = 0;
        quint64 indexOffset = 0;
        quint64 indexCount = 0;
    };

    struct RawSingletonClusterHeader
    {
        quint32 nameOffset = 0;
        quint32 nameSize = 0;
        quint32 idOffset = 0;
        quint32 idSize = 0;
        quint32 rgba = 0;
        quint32 index = 0; // Actual point index, not index-buffer offset.
    };
#pragma pack(pop)

    static_assert(std::is_trivially_copyable_v<RawClusterHeader>);
    static_assert(std::is_trivially_copyable_v<RawSingletonClusterHeader>);

    void throwIfTooLargeForQSizeType(quint64 value, const char* what)
    {
        if (value > static_cast<quint64>(std::numeric_limits<qsizetype>::max()))
            throw std::runtime_error(what);
    }

    quint32 appendUtf8String(QByteArray& stringBlob, const QByteArray& stringBytes)
    {
        if (stringBlob.size() > std::numeric_limits<quint32>::max())
            throw std::runtime_error("Cluster string blob too large");

        const auto offset = static_cast<quint32>(stringBlob.size());

        if (stringBytes.size() > std::numeric_limits<quint32>::max())
            throw std::runtime_error("Cluster string too large");

        const auto finalSize =
            static_cast<quint64>(stringBlob.size()) +
            static_cast<quint64>(stringBytes.size());

        if (finalSize > std::numeric_limits<quint32>::max())
            throw std::runtime_error("Cluster string blob too large");

        stringBlob.append(stringBytes);

        return offset;
    }

    void validateStringRange(quint32 offset, quint32 size, quint64 blobSize)
    {
        const auto end =
            static_cast<quint64>(offset) +
            static_cast<quint64>(size);

        if (end > blobSize)
            throw std::runtime_error("Cluster string range exceeds string blob");
    }
}

UniqueWorkflowPlan ClustersSerializer::fromVariantMapWorkflow(
    const QVariantMap& map,
    QVector<Cluster>& clusters,
    SharedWorkflowExecutionContext executionContext)
{
    struct Context {
        std::vector<Header> headers;
        Indices allIndices;
    };

    auto context = std::make_shared<Context>();
    auto plan = std::make_unique<WorkflowPlan>(__FUNCTION__);

    plan->addSequentialStage("Read cluster data",
        [map, context](const WorkflowPlan::Job&,
            const SharedWorkflowExecutionContext& parentExecutionContext) {
                const auto version = map.value("ClustersFormatVersion").toUInt();

                if (version != FormatVersion)
                    throw std::runtime_error("Unsupported cluster serialization format version");

                const auto metaData = bytesFromBlobVariantMap(
                    map.value("ClustersMetaData").toMap(),
                    parentExecutionContext);

                auto decoded = deserializeHeaders(metaData);

                context->headers = std::move(decoded.headers);

                if (decoded.hasEmbeddedIndices) {
                    context->allIndices = std::move(decoded.embeddedIndices);
                    return;
                }

                const auto indicesRawData = bytesFromBlobVariantMap(
                    map.value("ClustersIndicesRawData").toMap(),
                    parentExecutionContext);

                if ((indicesRawData.size() % static_cast<qsizetype>(sizeof(unsigned int))) != 0)
                    throw std::runtime_error("Invalid cluster index raw data size");

                context->allIndices.resize(
                    static_cast<std::size_t>(indicesRawData.size()) / sizeof(unsigned int));

                std::memcpy(
                    context->allIndices.data(),
                    indicesRawData.constData(),
                    static_cast<std::size_t>(indicesRawData.size()));
        });

    plan->addSequentialStage("Prepare clusters",
        [context, &clusters](const WorkflowPlan::Job&,
            const SharedWorkflowExecutionContext&) {
                clusters.resize(static_cast<qsizetype>(context->headers.size()));
        });
    /*
    WorkflowPlan::Jobs rebuildJobs;

    // Important: this loop cannot use context->headers.size() yet,
    // because headers are loaded only when the workflow runs.
    // So instead use a dynamic nested workflow stage:

    plan->addNestedWorkflowStage("Rebuild clusters",
        [context, &clusters](const WorkflowPlan::Job&,
            const SharedWorkflowExecutionContext&) -> UniqueWorkflowPlan {
                auto rebuildPlan = std::make_unique<WorkflowPlan>("Rebuild clusters");

                WorkflowPlan::Jobs rebuildJobs;
                rebuildJobs.reserve(context->headers.size());

                for (std::size_t i = 0; i < context->headers.size(); ++i) {
                    rebuildJobs.emplace_back(
                        QString("Rebuild cluster %1").arg(i),
                        [context, &clusters, i](const WorkflowPlan::Job&,
                            const SharedWorkflowExecutionContext&) {
                                const Header& header = context->headers[i];

                                const auto offset = static_cast<std::size_t>(header.indexOffset);
                                const auto count = static_cast<std::size_t>(header.indexCount);

                                if (offset > context->allIndices.size() ||
                                    count > context->allIndices.size() - offset) {
                                    throw std::runtime_error("Cluster index range exceeds index buffer");
                                }

                                Cluster cluster;
                                cluster.setName(header.name);
                                cluster.setId(header.id);
                                cluster.setColor(header.color);

                                std::vector<unsigned int> indices(
                                    context->allIndices.begin() + offset,
                                    context->allIndices.begin() + offset + count);

                                cluster.setIndices(std::move(indices));

                                clusters[static_cast<qsizetype>(i)] = std::move(cluster);
                        });
                }

                rebuildPlan->addBatchedParallelStage(
                    "Rebuild clusters",
                    std::move(rebuildJobs),
                    8);

                return rebuildPlan;
        });
*/
    return plan;
}

QVariantMap ClustersSerializer::toVariantMapScoped(const QVector<Cluster>& clusters, SharedWorkflowExecutionContext executionContext)
{
    Headers headers;
    Indices allIndices;
    QVariantMap result;

    headers.reserve(static_cast<std::size_t>(clusters.size()));

    allIndices = buildIndexBuffer(clusters, headers);

    if (executionContext && headers.size() >= 1000) {
        executionContext->warning(
            QString("This dataset contains approximately %1 clusters. "
                "Datasets with very large numbers of clusters can take considerable "
                "time to save and load. Consider reducing the number of clusters if "
                "project serialization performance becomes a concern.")
            .arg(getIntegerCountHumanReadable(headers.size())));

        executionContext->info("root warning count:" + QString::number(executionContext->getState()->getReportRoot()->getWarningCountRecursive()));

        for (auto message : executionContext->getState()->collectMessages()) {
            qDebug() << message._text;
        }


    }

    const auto headersRaw = serializeHeaders(headers, allIndices);

    result.insert("ClustersFormatVersion", FormatVersion);
    result.insert("ClustersMetaDataSize", headersRaw.size());
    result.insert("ClustersIndicesRawDataSize", allIndices.size() * sizeof(unsigned int));

    result.insert(
        "ClustersMetaData",
        bytesToBlobVariantMap(headersRaw.constData(), headersRaw.size(), executionContext)
    );

    result.insert(
        "ClustersIndicesRawData",
        bytesToBlobVariantMap(
            reinterpret_cast<const char*>(allIndices.data()),
            allIndices.size() * sizeof(unsigned int),
            executionContext
        )
    );

    return result;
}

bool ClustersSerializer::canUseSingletonHeaderFormat(const Headers& headers, const Indices& allIndices)
{
    return std::all_of(headers.begin(), headers.end(), [&allIndices](const Header& header) {
        if (header.indexCount != 1)
            return false;

        const auto offset = static_cast<std::size_t>(header.indexOffset);

        if (offset >= allIndices.size())
            return false;

        return allIndices[offset] <= std::numeric_limits<quint32>::max();
    });
}

QByteArray ClustersSerializer::serializeHeaders(
    const Headers& headers,
    const Indices& allIndices)
{
    if (canUseSingletonHeaderFormat(headers, allIndices))
        return serializeSingletonHeaders(headers, allIndices);

    return serializeGeneralHeaders(headers);
}

QByteArray ClustersSerializer::serializeGeneralHeaders(const Headers& headers)
{
    if (headers.size() > std::numeric_limits<quint32>::max())
        throw std::runtime_error("Too many cluster headers");

    std::vector<RawClusterHeader> rawHeaders;
    rawHeaders.reserve(headers.size());

    QByteArray stringBlob;

    for (const auto& header : headers) {
        RawClusterHeader raw;

        const auto nameBytes = header.name.toUtf8();
        const auto idBytes = header.id.toUtf8();

        raw.nameOffset = appendUtf8String(stringBlob, nameBytes);
        raw.nameSize = static_cast<quint32>(nameBytes.size());

        raw.idOffset = appendUtf8String(stringBlob, idBytes);
        raw.idSize = static_cast<quint32>(idBytes.size());

        raw.rgba = header.color.rgba();
        raw.indexOffset = header.indexOffset;
        raw.indexCount = header.indexCount;

        rawHeaders.emplace_back(raw);
    }

    QByteArray bytes;
    QDataStream stream(&bytes, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_6_5);

    const auto headerBytes =
        static_cast<quint64>(rawHeaders.size() * sizeof(RawClusterHeader));

    stream
        << static_cast<quint32>(ClusterHeaderStorageMode::General)
        << static_cast<quint32>(rawHeaders.size())
        << static_cast<quint32>(sizeof(RawClusterHeader))
        << headerBytes
        << static_cast<quint64>(stringBlob.size());

    if (headerBytes > 0) {
        const auto written = stream.writeRawData(
            reinterpret_cast<const char*>(rawHeaders.data()),
            static_cast<qsizetype>(headerBytes)
        );

        if (written != static_cast<qsizetype>(headerBytes))
            throw std::runtime_error("Failed to write raw cluster headers");
    }

    if (!stringBlob.isEmpty()) {
        const auto written = stream.writeRawData(
            stringBlob.constData(),
            stringBlob.size()
        );

        if (written != stringBlob.size())
            throw std::runtime_error("Failed to write cluster string blob");
    }

    if (stream.status() != QDataStream::Ok)
        throw std::runtime_error("Failed to serialize cluster headers");

    return bytes;
}

QByteArray ClustersSerializer::serializeSingletonHeaders(
    const Headers& headers,
    const Indices& allIndices)
{
    if (headers.size() > std::numeric_limits<quint32>::max())
        throw std::runtime_error("Too many singleton cluster headers");

    std::vector<RawSingletonClusterHeader> rawHeaders;
    rawHeaders.reserve(headers.size());

    QByteArray stringBlob;

    for (const auto& header : headers) {
        if (header.indexCount != 1)
            throw std::runtime_error("Non-singleton header in singleton serializer");

        const auto offset = static_cast<std::size_t>(header.indexOffset);

        if (offset >= allIndices.size())
            throw std::runtime_error("Singleton cluster index offset out of bounds");

        const auto actualIndex = allIndices[offset];

        if (actualIndex > std::numeric_limits<quint32>::max())
            throw std::runtime_error("Singleton cluster index exceeds quint32 range");

        RawSingletonClusterHeader raw;

        const auto nameBytes = header.name.toUtf8();
        const auto idBytes = header.id.toUtf8();

        raw.nameOffset = appendUtf8String(stringBlob, nameBytes);
        raw.nameSize = static_cast<quint32>(nameBytes.size());

        raw.idOffset = appendUtf8String(stringBlob, idBytes);
        raw.idSize = static_cast<quint32>(idBytes.size());

        raw.rgba = header.color.rgba();
        raw.index = static_cast<quint32>(actualIndex);

        rawHeaders.emplace_back(raw);
    }

    QByteArray bytes;
    QDataStream stream(&bytes, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_6_5);

    const auto headerBytes =
        static_cast<quint64>(rawHeaders.size() * sizeof(RawSingletonClusterHeader));

    stream
        << static_cast<quint32>(ClusterHeaderStorageMode::Singleton)
        << static_cast<quint32>(rawHeaders.size())
        << static_cast<quint32>(sizeof(RawSingletonClusterHeader))
        << headerBytes
        << static_cast<quint64>(stringBlob.size());

    if (headerBytes > 0) {
        const auto written = stream.writeRawData(
            reinterpret_cast<const char*>(rawHeaders.data()),
            static_cast<qsizetype>(headerBytes)
        );

        if (written != static_cast<qsizetype>(headerBytes))
            throw std::runtime_error("Failed to write raw singleton cluster headers");
    }

    if (!stringBlob.isEmpty()) {
        const auto written = stream.writeRawData(
            stringBlob.constData(),
            stringBlob.size()
        );

        if (written != stringBlob.size())
            throw std::runtime_error("Failed to write singleton cluster string blob");
    }

    if (stream.status() != QDataStream::Ok)
        throw std::runtime_error("Failed to serialize singleton cluster headers");

    return bytes;
}

ClustersSerializer::DecodedHeaders ClustersSerializer::deserializeHeaders(const QByteArray& bytes)
{
    QDataStream stream(bytes);
    stream.setVersion(QDataStream::Qt_6_5);

    quint32 modeValue = 0;
    quint32 count = 0;
    quint32 recordSize = 0;
    quint64 headerBytes = 0;
    quint64 stringBytes = 0;

    stream
        >> modeValue
        >> count
        >> recordSize
        >> headerBytes
        >> stringBytes;

    if (stream.status() != QDataStream::Ok)
        throw std::runtime_error("Failed to read cluster header preamble");

    const auto mode = static_cast<ClusterHeaderStorageMode>(modeValue);

    if (mode == ClusterHeaderStorageMode::General) {
        return deserializeGeneralHeaders(
            stream,
            count,
            recordSize,
            headerBytes,
            stringBytes
        );
    }

    if (mode == ClusterHeaderStorageMode::Singleton) {
        return deserializeSingletonHeaders(
            stream,
            count,
            recordSize,
            headerBytes,
            stringBytes
        );
    }

    throw std::runtime_error("Unsupported cluster header storage mode");
}

ClustersSerializer::DecodedHeaders ClustersSerializer::deserializeGeneralHeaders(QDataStream& stream, quint32 count, quint32 recordSize, quint64 headerBytes, quint64 stringBytes)
{
    if (recordSize != sizeof(RawClusterHeader))
        throw std::runtime_error("Unsupported general cluster header record size");

    if (headerBytes != static_cast<quint64>(count) * sizeof(RawClusterHeader))
        throw std::runtime_error("Invalid general cluster header byte size");

    throwIfTooLargeForQSizeType(headerBytes, "General cluster header block too large");
    throwIfTooLargeForQSizeType(stringBytes, "General cluster string blob too large");

    std::vector<RawClusterHeader> rawHeaders(count);

    if (headerBytes > 0) {
        const auto read = stream.readRawData(
            reinterpret_cast<char*>(rawHeaders.data()),
            static_cast<qsizetype>(headerBytes)
        );

        if (read != static_cast<qsizetype>(headerBytes))
            throw std::runtime_error("Failed to read raw cluster headers");
    }

    QByteArray stringBlob;
    stringBlob.resize(static_cast<qsizetype>(stringBytes));

    if (stringBytes > 0) {
        const auto read = stream.readRawData(
            stringBlob.data(),
            stringBlob.size()
        );

        if (read != stringBlob.size())
            throw std::runtime_error("Failed to read cluster string blob");
    }

    DecodedHeaders decoded;
    decoded.headers.reserve(count);
    decoded.hasEmbeddedIndices = false;

    const auto blobSize = static_cast<quint64>(stringBlob.size());

    for (const auto& raw : rawHeaders) {
        validateStringRange(raw.nameOffset, raw.nameSize, blobSize);
        validateStringRange(raw.idOffset, raw.idSize, blobSize);

        Header header;

        header.name = QString::fromUtf8(
            stringBlob.constData() + raw.nameOffset,
            static_cast<qsizetype>(raw.nameSize)
        );

        header.id = QString::fromUtf8(
            stringBlob.constData() + raw.idOffset,
            static_cast<qsizetype>(raw.idSize)
        );

        header.color = QColor::fromRgba(raw.rgba);
        header.indexOffset = raw.indexOffset;
        header.indexCount = raw.indexCount;

        decoded.headers.emplace_back(std::move(header));
    }

    return decoded;
}

ClustersSerializer::DecodedHeaders ClustersSerializer::deserializeSingletonHeaders(QDataStream& stream, quint32 count, quint32 recordSize, quint64 headerBytes, quint64 stringBytes)
{
    if (recordSize != sizeof(RawSingletonClusterHeader))
        throw std::runtime_error("Unsupported singleton cluster header record size");

    if (headerBytes != static_cast<quint64>(count) * sizeof(RawSingletonClusterHeader))
        throw std::runtime_error("Invalid singleton cluster header byte size");

    throwIfTooLargeForQSizeType(headerBytes, "Singleton cluster header block too large");
    throwIfTooLargeForQSizeType(stringBytes, "Singleton cluster string blob too large");

    std::vector<RawSingletonClusterHeader> rawHeaders(count);

    if (headerBytes > 0) {
        const auto read = stream.readRawData(
            reinterpret_cast<char*>(rawHeaders.data()),
            static_cast<qsizetype>(headerBytes)
        );

        if (read != static_cast<qsizetype>(headerBytes))
            throw std::runtime_error("Failed to read raw singleton cluster headers");
    }

    QByteArray stringBlob;
    stringBlob.resize(static_cast<qsizetype>(stringBytes));

    if (stringBytes > 0) {
        const auto read = stream.readRawData(
            stringBlob.data(),
            stringBlob.size()
        );

        if (read != stringBlob.size())
            throw std::runtime_error("Failed to read singleton cluster string blob");
    }

    DecodedHeaders decoded;
    decoded.headers.reserve(count);
    decoded.embeddedIndices.reserve(count);
    decoded.hasEmbeddedIndices = true;

    const auto blobSize = static_cast<quint64>(stringBlob.size());

    for (const auto& raw : rawHeaders) {
        validateStringRange(raw.nameOffset, raw.nameSize, blobSize);
        validateStringRange(raw.idOffset, raw.idSize, blobSize);

        Header header;

        header.name = QString::fromUtf8(
            stringBlob.constData() + raw.nameOffset,
            static_cast<qsizetype>(raw.nameSize)
        );

        header.id = QString::fromUtf8(
            stringBlob.constData() + raw.idOffset,
            static_cast<qsizetype>(raw.idSize)
        );

        header.color = QColor::fromRgba(raw.rgba);

        header.indexOffset = static_cast<quint64>(decoded.embeddedIndices.size());
        header.indexCount = 1;

        decoded.embeddedIndices.emplace_back(raw.index);
        decoded.headers.emplace_back(std::move(header));
    }

    return decoded;
}

std::vector<unsigned int> ClustersSerializer::buildIndexBuffer(
    const QVector<Cluster>& clusters,
    std::vector<Header>& headers)
{
    std::size_t totalIndexCount = 0;

    for (const Cluster& cluster : clusters)
        totalIndexCount += cluster.getIndices().size();

    std::vector<unsigned int> allIndices;
    allIndices.reserve(totalIndexCount);

    for (const Cluster& cluster : clusters) {
        Header header;

        header.name = cluster.getName();
        header.id = cluster.getId();
        header.color = cluster.getColor();

        const auto& indices = cluster.getIndices();

        header.indexOffset = static_cast<quint64>(allIndices.size());
        header.indexCount = static_cast<quint64>(indices.size());

        allIndices.insert(
            allIndices.end(),
            indices.begin(),
            indices.end()
        );

        headers.emplace_back(std::move(header));
    }

    return allIndices;
}
/*
QVector<Cluster> ClustersSerializer::rebuildClusters(
    const std::vector<Header>& headers,
    const std::vector<unsigned int>& allIndices)
{
    const auto clusterCount = headers.size();

    std::uint64_t totalClusterIndices = 0;

    for (const Header& header : headers) {
        const auto offset = static_cast<std::size_t>(header.indexOffset);
        const auto count = static_cast<std::size_t>(header.indexCount);

        if (offset > allIndices.size() || count > allIndices.size() - offset)
            throw std::runtime_error("Cluster index range exceeds index buffer");

        totalClusterIndices += count;
    }

    constexpr std::size_t minParallelClusters = 8;
    constexpr std::uint64_t minParallelIndices = 250'000;

    const bool useParallel =
        clusterCount >= minParallelClusters &&
        totalClusterIndices >= minParallelIndices;

    QVector<Cluster> clusters;
    clusters.resize(static_cast<qsizetype>(clusterCount));

    auto rebuildOne = [&](std::size_t i) {
        const Header& header = headers[i];

        const auto offset = static_cast<std::size_t>(header.indexOffset);
        const auto count = static_cast<std::size_t>(header.indexCount);

        Cluster cluster;

        cluster.setName(header.name);
        cluster.setId(header.id);
        cluster.setColor(header.color);

        std::vector<unsigned int> indices(
            allIndices.begin() + offset,
            allIndices.begin() + offset + count
        );

        cluster.setIndices(std::move(indices));

        clusters[static_cast<qsizetype>(i)] = std::move(cluster);
        };

    if (!useParallel) {
        for (std::size_t i = 0; i < clusterCount; ++i)
            rebuildOne(i);

        return clusters;
    }

    std::vector<std::size_t> indices(clusterCount);

    std::iota(indices.begin(), indices.end(), std::size_t{ 0 });

    //QtConcurrent::blockingMap(indices, rebuildOne);

    return clusters;
}
*/