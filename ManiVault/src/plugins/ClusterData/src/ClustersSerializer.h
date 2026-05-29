// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ClusterData.h"

/**
 * @brief The ClustersSerializer utility class provides functionality to serialize and deserialize Cluster objects to and from QVariantMaps using workflow plans.
 * @authors Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class ClustersSerializer : public QObject
{
protected:

    /** Struct representing the header information of a cluster, including its name, id, color, indices offset and count, and statistical measures (median, mean, stddev) for the cluster's data. This struct is used to store and manage the metadata associated with each cluster during the serialization and deserialization processes. */
    struct Header
    {
        QString                 name;               /** The name of the cluster, which will be serialized as part of the header information for each cluster.*/
        QString                 id;                 /** The unique identifier for the cluster, which will be serialized as part of the header information for each cluster. */
        QColor                  color;              /** The color of the cluster, which will be serialized as part of the header information for each cluster. */
        quint64                 indexOffset = 0;    /** The offset of the cluster's indices in the serialized indices data, which will be used during serialization to determine where to read/write the indices for each cluster. */
        quint64                 indexCount = 0;     /** The number of indices that belong to the cluster, which will be used during serialization to determine how many indices to read/write for each cluster. */
    };

    

public:

    /** The format version number for the cluster serialization. This constant is used to indicate the version of the serialization format being used, allowing for compatibility checks and potential future updates to the serialization logic while maintaining backward compatibility with older versions. */
    static constexpr quint32 FormatVersion = 2;

    /**
     * @brief Creates a workflow plan that deserializes a QVariantMap into a vector of Cluster objects.
     * @param map The QVariantMap that contains the serialized cluster data that needs to be deserialized into a vector of Cluster objects.
     * @param clusters The vector of Cluster objects that will be populated with the deserialized data from the QVariantMap during the workflow execution.
     * @param parentExecutionContext The shared workflow execution context that will be passed to the workflow plan during its execution.
     * @return A UniqueWorkflowPlan object that represents the workflow plan for deserializing the given QVariantMap into a vector of Cluster objects.
     */
    static void fromVariantMapScoped(const QVariantMap& map, QVector<Cluster>& clusters, SharedWorkflowExecutionContext parentExecutionContext);

    
    static QVariantMap toVariantMapScoped(const QVector<Cluster>& clusters, SharedWorkflowExecutionContext parentExecutionContext);

    

    using Headers = std::vector<Header>;
    using Indices = std::vector<unsigned int>;
    using HeadersRaw = QByteArray;

    struct DecodedHeaders
    {
        Headers headers;
        Indices embeddedIndices;
        bool hasEmbeddedIndices = false;
    };

    static bool canUseSingletonHeaderFormat(const Headers& headers, const Indices& allIndices);
    static QByteArray serializeHeaders(const Headers& headers, const Indices& allIndices);
    static QByteArray serializeGeneralHeaders(const Headers& headers);
    static QByteArray serializeSingletonHeaders(const Headers& headers, const Indices& allIndices);

    
      

private:

    

    static DecodedHeaders deserializeHeaders(const QByteArray& bytes);
    static DecodedHeaders deserializeGeneralHeaders(QDataStream& stream, quint32 count, quint32 recordSize, quint64 headerBytes, quint64 stringBytes);
    static DecodedHeaders deserializeSingletonHeaders(QDataStream& stream, quint32 count, quint32 recordSize, quint64 headerBytes, quint64 stringBytes);

    /**
     * @brief Builds an index buffer by concatenating the indices from all clusters and creates corresponding headers with updated index offsets and counts.
     * @param clusters The vector of Cluster objects, where each cluster contains a vector of indices.
     * @param headers The vector of Header structures that will be populated with the updated index offsets and counts for each cluster.
     * @return A vector of unsigned int containing the concatenated indices from all clusters.
     */
    static std::vector<unsigned int> buildIndexBuffer(const QVector<Cluster>& clusters, std::vector<Header>& headers);

    /**
     * @brief Reconstructs a vector of Cluster objects from the provided headers and index buffer.
     * @param headers The vector of Header structures containing information about each cluster, including name, id, color, index offset, index count, median, mean, and standard deviation.
     * @param allIndices The vector of unsigned int containing the concatenated indices for all clusters, where the indices for each cluster can be determined using the index offsets and counts specified in the headers.
     * @return A QVector of Cluster objects reconstructed from the provided headers and index buffer, where each Cluster object is populated with its corresponding name, id, color, indices, median, mean, and standard deviation based on the information in the headers and the indices in the index buffer.
     */
    static QVector<Cluster> rebuildClusters(const std::vector<Header>& headers, const std::vector<unsigned int>& allIndices);
};