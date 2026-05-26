// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <util/WorkflowContextBase.h>

#include "ClusterData.h"

/**
 * @brief The ClustersSerializer utility class provides functionality to serialize and deserialize Cluster objects to and from QVariantMaps using workflow plans.
 * @authors Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class ClustersSerializer : public QObject
{
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
    static UniqueWorkflowPlan fromVariantMapWorkflow(const QVariantMap& map, QVector<Cluster>& clusters, SharedWorkflowExecutionContext parentExecutionContext);

    /**
     * @brief Creates a workflow plan that serializes the given clusters into a QVariantMap.
     * @param clusters The vector of Cluster objects that need to be serialized into a QVariantMap.
     * @param parentExecutionContext The shared workflow execution context that will be passed to the workflow plan during its execution.
     * @return A UniqueWorkflowPlan object that represents the workflow plan for serializing the given clusters into a QVariantMap.
     */
    static UniqueWorkflowPlan toVariantMapWorkflow(const QVector<Cluster>& clusters, SharedWorkflowExecutionContext parentExecutionContext);

protected:

    /** Struct representing the header information of a cluster, including its name, id, color, indices offset and count, and statistical measures (median, mean, stddev) for the cluster's data. This struct is used to store and manage the metadata associated with each cluster during the serialization and deserialization processes. */
    struct Header
    {
        QString                 name;               /** The name of the cluster, which will be serialized as part of the header information for each cluster.*/
        QString                 id;                 /** The unique identifier for the cluster, which will be serialized as part of the header information for each cluster. */
        QColor                  color;              /** The color of the cluster, which will be serialized as part of the header information for each cluster. */
        quint64                 indexOffset = 0;    /** The offset of the cluster's indices in the serialized indices data, which will be used during serialization to determine where to read/write the indices for each cluster. */
        quint64                 indexCount = 0;     /** The number of indices that belong to the cluster, which will be used during serialization to determine how many indices to read/write for each cluster. */
        std::vector<float>      median;             /** Median values for the cluster's data (will become re-located in the future) */
        std::vector<float>      mean;               /** Mean values for the cluster's data (will become re-located in the future) */
        std::vector<float>      stddev;             /** Standard deviation values for the cluster's data (will become re-located in the future) */
    };

    using Headers       = std::vector<Header>;
    using Indices       = std::vector<unsigned int>;
    using HeadersRaw    = QByteArray;

    /** Context used during the toVariantMapWorkflow execution to store intermediate data between workflow stages */
    struct ToVariantMapWorkflowContext : public WorkflowContextBase
    {
    public:

        /**
         * @brief Gets the headers from the context.
         * @return The headers stored in the context
         */
        Headers getHeaders() const {
            QMutexLocker locker(&_mutex);
            return _headers;
        }

        /**
         * @brief Sets the headers in the context.
         * @param headers The headers to be set in the context
         */
        void setHeaders(Headers headers) {
            QMutexLocker locker(&_mutex);
            _headers = std::move(headers);
        }

        /**
         * @brief Gets the indices from the context.
         * @return The indices stored in the context
         */
        const Indices& getAllIndices() const {
            QMutexLocker locker(&_mutex);
            return _allIndices;
        }

        /**
         * @brief Gets the indices from the context.
         * @return The indices stored in the context
         */
        qsizetype getAllIndicesRawSize() const {
            QMutexLocker locker(&_mutex);
            return _allIndices.size() * sizeof(unsigned int);
        }

        /**
         * @brief Sets the indices in the context.
         * @param allIndices The indices to be set in the context
         */
        void setAllIndices(Indices allIndices) {
            QMutexLocker locker(&_mutex);
            _allIndices = std::move(allIndices);
        }

        /**
         * @brief Gets the headers from the context.
         * @return The headers stored in the context
         */
        void setHeadersRaw(HeadersRaw headersRaw) {
            QMutexLocker locker(&_mutex);
            _headersRaw = std::move(headersRaw);
        }

        /**
         * @brief Gets the headers from the context.
         * @return The headers stored in the context
         */
        HeadersRaw getHeadersRaw() const {
            QMutexLocker locker(&_mutex);
            return _headersRaw;
        }

        /**
         * @brief Gets the headers from the context.
         * @return The headers stored in the context
         */
        const HeadersRaw& getHeadersRaw() {
            QMutexLocker locker(&_mutex);
            return _headersRaw;
        }

        /**
         * @brief Gets the size of the raw headers stored in the context.
         * @return The size of the raw headers stored in the context
         */
        qsizetype getHeadersRawSize() const {
            QMutexLocker locker(&_mutex);
            return _headersRaw.size();
        }

        /**
         * @brief Gets the indices from the context.
         * @return The indices stored in the context
         */
        QVariantMap getResult() const {
            QMutexLocker locker(&_mutex);
            return _result;
        }

        /**
         * @brief Sets the result in the context.
         * @param result The result to be set in the context
         */
        void setResult(QVariantMap result) {
            QMutexLocker locker(&_mutex);
            _result = std::move(result);
        }

        /**
         * @brief Sets a key-value pair in the result map of the context.
         * @param key The key to be set in the result map
         * @param value The value to be associated with the key in the result map
         */
        void setResultValue(const QString& key, const QVariant& value) {
            QMutexLocker locker(&_mutex);
            _result.insert(key, value);
        }

    private:
        mutable QMutex      _mutex;         /** Mutex for synchronizing access to the context */
        Headers             _headers;       /** The headers extracted from the clusters during the workflow execution. */
        Indices             _allIndices;    /** The indices extracted from the clusters during the workflow execution. */
        HeadersRaw          _headersRaw;    /** The serialized headers as a QByteArray, which will be stored in the QVariantMap for later deserialization during the fromVariantMapWorkflow execution. */
        QVariantMap         _result;         /** The QVariantMap that will hold the final result of the workflow execution, containing the serialized headers and indices data. This map will be returned at the end of the workflow execution and can be used for further processing or storage. */
    };

    /** Context used during the fromVariantMapWorkflow execution to store intermediate data between workflow stages */
    class FromVariantMapWorkflowContext : public WorkflowContextBase
    {
    public:
	    /**
         * @brief Constructs a FromVariantMapWorkflowContext with the provided QVariantMap.
         * @param map The QVariantMap containing the data to be used during the workflow execution. This map is expected to contain the necessary information for loading clusters, such as serialized headers and indices data. The context will store this map for use in different stages of the workflow, allowing for the deserialization and reconstruction of clusters from the provided data.
	     */
	    explicit FromVariantMapWorkflowContext(QVariantMap map) :
            _map(std::move(map))
        {
        }

        /**
         * @brief Gets the headers from the context.
         * @return The headers stored in the context
         */
        const Headers& getHeaders() const {
            QMutexLocker locker(&_mutex);
            return _headers;
        }

        /**
         * @brief Sets the headers in the context.
         * @param headers The headers to be set in the context
         */
        void setHeaders(const Headers& headers) {
            QMutexLocker locker(&_mutex);
            _headers = headers;
        }

        /**
         * @brief Gets the indices from the context.
         * @return The indices stored in the context
         */
        const Indices& getAllIndices() const {
            QMutexLocker locker(&_mutex);
            return _allIndices;
        }

        /**
         * @brief Gets the headers from the context.
         * @return The headers stored in the context
         */
        void setAllIndices(const Indices& allIndices) {
            QMutexLocker locker(&_mutex);
            _allIndices = allIndices;
        }

    private:
        mutable QMutex      _mutex;         /** Mutex for synchronizing access to the context */
        QVariantMap         _map;           /** The QVariantMap containing the data for the workflow execution */
        Headers             _headers;       /** The deserialized headers from the QVariantMap */
        Indices             _allIndices;    /** The deserialized indices from the QVariantMap */
    };

private:

    /**
     * @brief Serializes the headers of the clusters into a QByteArray format.
     * @param headers The vector of headers to be serialized, where each header contains information about a cluster such as its name, id, color, index offset, index count, median, mean, and standard deviation.
     * @return A QByteArray containing the serialized headers data, which can be stored in a QVariantMap or used for other purposes where a compact binary representation of the headers is needed.
     */
    static QByteArray serializeHeaders(const std::vector<Header>& headers);

    /**
     * @brief Deserializes the headers from a QByteArray format back into a vector of Header structures.
     * @param bytes The QByteArray containing the serialized headers data, which was previously generated by the serializeHeaders method.
     * @return A vector of Header structures containing the deserialized headers data, which can be used for further processing or for reconstructing clusters during the workflow execution.
     */
    static std::vector<Header> deserializeHeaders(const QByteArray& bytes);

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