// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ClusterDataLegacySerialization.h"
#include "ClusterData.h"

#include <util/Serialization.h>

#include <workflow/WorkflowRuntimeScoped.h>

using namespace mv::util;

namespace mv::legacy
{

void ClusterDataLegacySerializer::fromVariantMapPre150(ClusterData& clusterData, const QVariantMap& variantMap, const workflow::SharedWorkflowExecutionContext& executionContext)
{
    executionContext->warning("Deserializing ClusterData from legacy format (pre-1.5.0)");

    const auto dataMap = variantMap["Data"].toMap();

    variantMapMustContain(dataMap, "IndicesRawData");
    variantMapMustContain(dataMap, "NumberOfIndices");

    // Packed indices for all clusters
    QVector<std::uint32_t> packedIndices;

    packedIndices.resize(dataMap["NumberOfIndices"].toInt());

    // Convert raw data to indices
    if (!packedIndices.empty())
		populateBytesFromBlobMap(dataMap["IndicesRawData"].toMap(), (char*)packedIndices.data(), packedIndices.size() * sizeof(std::uint32_t));

    if (dataMap.contains("ClustersRawData")) {
        QByteArray clustersByteArray;

        QDataStream clustersDataStream(&clustersByteArray, QIODevice::ReadOnly);

        const auto clustersRawDataSize = dataMap["ClustersRawDataSize"].toInt();

        clustersByteArray.resize(clustersRawDataSize);

        populateBytesFromBlobMap(dataMap["ClustersRawData"].toMap(), (char*)clustersByteArray.data(), clustersByteArray.size());

        QVariantList clusters;

        clustersDataStream >> clusters;

        clusterData._clusters.resize(clusters.count());

        long clusterIndex = 0;

        for (const auto& clusterVariant : clusters) {
            const auto clusterMap = clusterVariant.toMap();

            auto& cluster = clusterData._clusters[clusterIndex];

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

        clusterData._clusters.resize(clustersList.count());

        for (const auto& clusterVariant : clustersList) {
            const auto clusterMap = clusterVariant.toMap();
            const auto clusterIndex = clustersList.indexOf(clusterMap);

            auto& cluster = clusterData._clusters[clusterIndex];

            cluster.setName(clusterMap["Name"].toString());
            cluster.setId(clusterMap["ID"].toString());
            cluster.setColor(clusterMap["Color"].toString());

            const auto globalIndicesOffset  = clusterMap["GlobalIndicesOffset"].toInt();
            const auto numberOfIndices      = clusterMap["NumberOfIndices"].toInt();

            cluster.getIndices() = std::vector<std::uint32_t>(packedIndices.begin() + globalIndicesOffset, packedIndices.begin() + globalIndicesOffset + numberOfIndices);
        }
    }
}

void ClustersLegacySerializer::fromVariantMapPre150(Clusters& clusters, const QVariantMap& variantMap, const workflow::SharedWorkflowExecutionContext& executionContext)
{
    executionContext->warning(QString("Deserializing %1 Clusters from legacy format (pre-1.5.0)").arg(clusters.getGuiName()));

    ClusterDataLegacySerializer::fromVariantMapPre150(*clusters.getRawData<ClusterData>(), variantMap, executionContext);

    events().notifyDatasetDataChanged(clusters);
}

}
