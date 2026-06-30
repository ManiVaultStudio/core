// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SetLegacySerialization.h"

#include "Set.h"
#include "RawData.h"

#include "util/Serialization.h"

using namespace mv;
using namespace mv::util;

namespace mv::legacy
{

namespace
{
    QVariant loadQVariant(const QVariant& variant)
    {
        // check if variant contains a non-empty QVariantMap
        QVariantMap variantMap = variant.toMap();
        if (variantMap.empty())
            return variant;

        // check if it was stored on disk, if not just return it
        if (!variantMap.contains("QVariantOnDiskVersion"))
        {
            return variantMap;
        }

        // load the QVariant from disk
        quint32 version = variantMap["QVariantOnDiskVersion"].toUInt();
        Q_ASSERT(version == 1);


        // First check the size we need for a temporary buffer
        variantMapMustContain(variantMap, "Blocks");
        const auto blocks = variantMap["Blocks"].toList();
        uint64_t totalSize = 0;
        for (const auto& block : blocks)
        {
            // Get block variant map
            const auto map = block.toMap();
            variantMapMustContain(map, "Size");
            totalSize += map["Size"].value<uint64_t>();
        }
        // Next create a temporary buffer and load the data
        //std::vector<char> bytes(totalSize);
        //populateDataBufferFromVariantMap(variantMap, bytes.data());

        // Finally convert the data to a QVariant.
        QVariant result;;
        //QByteArray byteArray(bytes.data(), bytes.size());
        //QDataStream stream(&byteArray, QIODevice::ReadOnly);
        //stream >> result;
        return result;
    }
}

void SetLegacySerializer::fromVariantMapPre150(DatasetImpl& dataset, const QVariantMap& variantMap)
{
    // WidgetAction::fromVariantMap(variantMap);

    variantMapMustContain(variantMap, "Name");
    variantMapMustContain(variantMap, "Locked");
    variantMapMustContain(variantMap, "StorageType");
    variantMapMustContain(variantMap, "DataType");
    variantMapMustContain(variantMap, "Derived");
    //    variantMapMustContain(variantMap, "Full");
    variantMapMustContain(variantMap, "LinkedData");

    dataset.setText(variantMap["Name"].toString());
    dataset.setLocked(variantMap["Locked"].toBool());
    dataset.setStorageType(static_cast<DatasetImpl::StorageType>(variantMap["StorageType"].toInt()));

    assert(variantMap["DataType"].toString() == dataset.getDataType().getTypeString());

    if (variantMap["Derived"].toBool())
    {
        if (variantMap.contains("SourceDatasetID"))
            dataset.setSourceDataset(mv::data().getDataset(variantMap["SourceDatasetID"].toString()));
        else
            dataset.setSourceDataset(dataset.getParent());

        assert(dataset._sourceDataset.isValid());
    }

    // For backwards compatibility, check PluginVersion
    if (!(variantMap["PluginVersion"] == "No Version") && !variantMap["Full"].toBool())
    {
        if (variantMap.contains("FullDatasetID"))
            dataset.makeSubsetOf(mv::data().getDataset(variantMap["FullDatasetID"].toString()));
        else
            dataset.makeSubsetOf(dataset.getParent()->getFullDataset<mv::DatasetImpl>());

        assert(variantMap["PluginKind"].toString() == dataset.getRawData<mv::plugin::RawData>()->getKind());
        assert(dataset._fullDataset.isValid());
    }

    if (variantMap.contains("GroupIndex") && variantMap["GroupIndex"].toInt() >= 0)
        dataset.setGroupIndex(variantMap["GroupIndex"].toInt());

    if (variantMap.contains("MayUnderive"))
        dataset._mayUnderive = variantMap["MayUnderive"].toBool();

    if (variantMap.contains("Properties"))
    {
        dataset._properties = loadQVariant(variantMap["Properties"]).toMap();
    }

    if (dataset.getStorageType() == DatasetImpl::StorageType::Proxy && variantMap.contains("ProxyMembers")) {
        Datasets proxyMembers;

        for (const auto& proxyMemberGuid : variantMap["ProxyMembers"].toStringList())
            proxyMembers << mv::data().getDataset(proxyMemberGuid);

        dataset.setProxyMembers(proxyMembers);
    }

    for (const auto& linkedDataVariant : variantMap["LinkedData"].toList()) {
        LinkedData linkedData;

        linkedData.fromVariantMap(linkedDataVariant.toMap());

        dataset._linkedData.push_back(linkedData);
    }
}

}
