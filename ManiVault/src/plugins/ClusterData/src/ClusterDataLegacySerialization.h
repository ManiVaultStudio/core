// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include <QVariantMap>

class ClusterData;
class Clusters;

namespace mv::legacy
{

/**
 * Deserializes cluster data from a variant map in the format used by ManiVault versions prior to 1.5.0.
 * @param clusterData The cluster data object to populate with the deserialized data
 * @param variantMap A QVariantMap containing the serialized cluster data in the legacy format
 */
void clusterDataFromVariantMapPre150(ClusterData& clusterData, const QVariantMap& variantMap);

/**
 * Deserializes clusters from a variant map in the format used by ManiVault versions prior to 1.5.0.
 * @param clusters The clusters object to populate with the deserialized data
 * @param variantMap A QVariantMap containing the serialized clusters in the legacy format
 */
void clustersFromVariantMapPre150(Clusters& clusters, const QVariantMap& variantMap);

}