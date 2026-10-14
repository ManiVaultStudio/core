// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include <workflow/WorkflowExecutionContext.h>

#include <QVariantMap>

class ClusterData;
class Clusters;

namespace mv::legacy
{

/** Utility class for managing legacy serialization of cluster data */
class ClusterDataLegacySerializer final
{
public:

    /**
     * Deserializes cluster data from a variant map in the format used by ManiVault versions prior to 1.5.0.
     * @param clusterData The cluster data object to populate with the deserialized data
     * @param variantMap A QVariantMap containing the serialized cluster data in the legacy format
     */
    static void fromVariantMapPre150(ClusterData& clusterData, const QVariantMap& variantMap, const workflow::SharedWorkflowExecutionContext& executionContext);
};

/** Utility class for managing legacy serialization of clusters */
class ClustersLegacySerializer final
{
public:

	/**
	 * Deserializes clusters from a variant map in the format used by ManiVault versions prior to 1.5.0.
	 * @param clusters The clusters object to populate with the deserialized data
	 * @param variantMap A QVariantMap containing the serialized clusters in the legacy format
	 */
    static void fromVariantMapPre150(Clusters& clusters, const QVariantMap& variantMap, const workflow::SharedWorkflowExecutionContext& executionContext);
};

}