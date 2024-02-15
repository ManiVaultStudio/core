// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PointsMetaData.h"

using namespace mv;

PointsMetaData::PointsMetaData(const QString& serializationName /*= "PointsMetaData"*/) :
    DatasetMetaData(serializationName),
    _metaData()
{
}

void PointsMetaData::fromVariantMap(const QVariantMap& variantMap)
{
    DatasetMetaData::fromVariantMap(variantMap);
}

QVariantMap PointsMetaData::toVariantMap() const
{
    auto variantMap = DatasetMetaData::toVariantMap();

    return variantMap;
}
