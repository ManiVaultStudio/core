// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ClusterDataLegacySerialization.h"

namespace mv::legacy
{

void clusterDataFromVariantMapPre150(ClusterData& clusterData, const QVariantMap& variantMap)
{
    qDebug() << "Deserializing ClusterData from legacy format (pre-1.5.0). This may result in loss of information if the format has changed significantly since then.";
}

void clustersFromVariantMapPre150(Clusters& clusters, const QVariantMap& variantMap)
{
    qDebug() << "Deserializing Clusters from legacy format (pre-1.5.0). This may result in loss of information if the format has changed significantly since then.";
}

}
