// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <util/WorkflowContextBase.h>

#include "clusterdata_export.h"

#include "ClusterData.h"

class ClusterSerializer : public QObject
{
public:
    static constexpr quint32 FormatVersion = 2;

    static QVariantMap toVariantMap(const QVector<Cluster>& clusters);

    static UniqueWorkflowPlan fromVariantMapWorkflow(const QVariantMap& map, QVector<Cluster>& clusters, SharedWorkflowExecutionContext parentExecutionContext);

protected:

    struct Header
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

    struct ClustersLoadContext : public WorkflowContextBase
    {
        explicit ClustersLoadContext(QVariantMap map) :
            _map(std::move(map))
        {
        }

        QVariantMap _map;
        std::vector<Header> _headers;
        std::vector<unsigned int> _allIndices;
    };

private:
    

    static QByteArray serializeHeaders(const std::vector<Header>& headers);
    static std::vector<Header> deserializeHeaders(const QByteArray& bytes);

    static std::vector<unsigned int> buildIndexBuffer(
        const QVector<Cluster>& clusters,
        std::vector<Header>& headers);

    static QVector<Cluster> rebuildClusters(
        const std::vector<Header>& headers,
        const std::vector<unsigned int>& allIndices);
};