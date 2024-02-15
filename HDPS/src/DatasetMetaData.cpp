// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DatasetMetaData.h"

using namespace mv::util;

namespace mv
{

DatasetMetaData::DatasetMetaData(QObject* parent, const QString& title /*= "DatasetMetaData"*/) :
    WidgetAction(parent, title),
    _metaData()
{
}

QVariantMap& DatasetMetaData::getMetaData()
{
    return _metaData;
}

void DatasetMetaData::fromVariantMap(const QVariantMap& variantMap)
{
    Serializable::fromVariantMap(variantMap);
}

QVariantMap DatasetMetaData::toVariantMap() const
{
    auto variantMap = Serializable::toVariantMap();

    return variantMap;
}

}