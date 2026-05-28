// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ClustersSerializer.h"

#include <CoreInterface.h>
#include <util/Serialization.h>

#include <QDataStream>

#include <limits>
#include <stdexcept>
#include <cstring>

using namespace mv::util;

namespace
{
    template <typename T>
    concept RawStreamable = std::is_arithmetic_v<T> && std::is_trivially_copyable_v<T>;

    template <RawStreamable T>
    QDataStream& writeRawVector(QDataStream& out, const std::vector<T>& values)
    {
        if (values.size() > std::numeric_limits<quint32>::max()) {
            out.setStatus(QDataStream::WriteFailed);
            return out;
        }

        out << static_cast<quint32>(values.size());

        if (values.empty())
            return out;

        const auto byteSize = static_cast<qsizetype>(values.size()) * static_cast<qsizetype>(sizeof(T));
        const auto written  = out.writeRawData(reinterpret_cast<const char*>(values.data()), byteSize);

        if (written != byteSize)
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

        values.resize(size);

        if (size == 0)
            return in;

        const auto byteSize = static_cast<qsizetype>(size) * static_cast<qsizetype>(sizeof(T));
        const auto read     = in.readRawData(reinterpret_cast<char*>(values.data()), byteSize);

        if (read != byteSize) {
            values.clear();
            in.setStatus(QDataStream::ReadPastEnd);
        }

        return in;
    }
}

void ClustersSerializer::fromVariantMapScoped(const QVariantMap& map, QVector<Cluster>& clusters, SharedWorkflowExecutionContext parentExecutionContext)
{
    Q_UNUSED(parentExecutionContext)

	const auto version = map.value("ClustersFormatVersion").toUInt();

    if (version != FormatVersion)
        throw std::runtime_error("Unsupported cluster serialization format version");

    auto context = std::make_shared<FromVariantMapWorkflowContext>(map);
    auto plan = std::make_unique<WorkflowPlan>(__FUNCTION__, context);

    const auto metaData = bytesFromBlobVariantMap(map.value("ClustersMetaData").toMap());

    if (metaData.isEmpty()) {
	    throw std::runtime_error("Cluster headers data is empty");
    }

	auto headers = deserializeHeaders(metaData);

    const auto indicesRawData = bytesFromBlobVariantMap(map.value("ClustersIndicesRawData").toMap());

    if (indicesRawData.isEmpty())
        throw std::runtime_error("Cluster indices data is empty");

    if (indicesRawData.size() % static_cast<qsizetype>(sizeof(unsigned int)) != 0)
        throw std::runtime_error("Cluster indices blob size is not aligned");

    Indices allIndices;

    allIndices.resize(static_cast<std::size_t>(indicesRawData.size()) / sizeof(unsigned int));

    if (!allIndices.empty()) {
        std::memcpy(allIndices.data(), indicesRawData.constData(), static_cast<std::size_t>(indicesRawData.size()));
        context->setAllIndices(allIndices);
    }

    clusters = rebuildClusters(context->getHeaders(), context->getAllIndices());
}

QVariantMap ClustersSerializer::toVariantMap(const QVector<Cluster>& clusters)
{
    Headers     headers;
    Indices     allIndices;
    QVariantMap result;

    headers.reserve(static_cast<std::size_t>(clusters.size()));

    buildIndexBuffer(clusters, headers);

	const auto headersRaw = serializeHeaders(headers);

    result.insert("ClustersFormatVersion", FormatVersion);
    result.insert("ClustersMetaDataSize", headersRaw.size());
    result.insert("ClustersIndicesRawDataSize", allIndices.size() * sizeof(unsigned int));
    result.insert("ClustersMetaData", bytesToBlobVariantMap(headersRaw.constData(), headersRaw.size(), nullptr));
    result.insert("ClustersIndicesRawData", bytesToBlobVariantMap(reinterpret_cast<const char*>(allIndices.data()), allIndices.size() * sizeof(unsigned int), nullptr));

    return result;
}

QByteArray ClustersSerializer::serializeHeaders(const std::vector<Header>& headers)
{
    QByteArray bytes;

    QDataStream stream(&bytes, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_6_5);

    if (headers.size() > std::numeric_limits<quint32>::max()) {
        stream.setStatus(QDataStream::WriteFailed);
        return bytes;
    }

    stream << static_cast<quint32>(headers.size());

    for (const Header& header : headers) {
        stream
            << header.name
            << header.id
            << header.color
            << header.indexOffset
            << header.indexCount;

        writeRawVector(stream, header.median);
        writeRawVector(stream, header.mean);
        writeRawVector(stream, header.stddev);

        if (stream.status() != QDataStream::Ok)
            break;
    }

    if (stream.status() != QDataStream::Ok)
        throw std::runtime_error("Failed to serialize cluster headers");

    return bytes;
}

std::vector<ClustersSerializer::Header>
ClustersSerializer::deserializeHeaders(const QByteArray& bytes)
{
    QDataStream stream(bytes);
    stream.setVersion(QDataStream::Qt_6_5);

    quint32 count = 0;
    stream >> count;

    if (stream.status() != QDataStream::Ok)
        throw std::runtime_error("Failed to read cluster header count");

    std::vector<Header> headers;
    headers.reserve(count);

    for (quint32 i = 0; i < count; ++i) {
        Header header;

        stream
            >> header.name
            >> header.id
            >> header.color
            >> header.indexOffset
            >> header.indexCount;

        readRawVector(stream, header.median);
        readRawVector(stream, header.mean);
        readRawVector(stream, header.stddev);

        if (stream.status() != QDataStream::Ok)
            throw std::runtime_error("Failed to deserialize cluster header");

        headers.emplace_back(std::move(header));
    }

    return headers;
}

std::vector<unsigned int> ClustersSerializer::buildIndexBuffer(const QVector<Cluster>& clusters, std::vector<Header>& headers)
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

        header.indexOffset = static_cast<quint64>(allIndices.size());
        header.indexCount =
            static_cast<quint64>(cluster.getIndices().size());

        header.median = cluster.getMedian();
        header.mean = cluster.getMean();
        header.stddev = cluster.getStandardDeviation();

        allIndices.insert(
            allIndices.end(),
            cluster.getIndices().begin(),
            cluster.getIndices().end()
        );

        headers.emplace_back(std::move(header));
    }

    return allIndices;
}

QVector<Cluster> ClustersSerializer::rebuildClusters(const std::vector<Header>& headers, const std::vector<unsigned int>& allIndices)
{
    QVector<Cluster> clusters;
    clusters.reserve(static_cast<qsizetype>(headers.size()));

    for (const Header& header : headers) {
        const auto offset   = static_cast<std::size_t>(header.indexOffset);
        const auto count    = static_cast<std::size_t>(header.indexCount);

        if (offset > allIndices.size() || count > allIndices.size() - offset)
            throw std::runtime_error("Cluster index range exceeds index buffer");

        Cluster cluster;

        cluster.setName(header.name);
        cluster.setId(header.id);
        cluster.setColor(header.color);
        //cluster.setMedian(header.median);
        //cluster.setMean(header.mean);
        //cluster.setStandardDeviation(header.stddev);

        auto indices = std::vector<unsigned int>(
            allIndices.begin() + offset,
            allIndices.begin() + offset + count
        );

        cluster.setIndices(indices);

        clusters.emplace_back(std::move(cluster));
    }

    return clusters;
}