#include "SelectionGroup.h"

#include "util/Serialization.h"

#include <QStringList>
#include <QVector>

#include <stdexcept>

namespace mv
{
    void BiMap::addKeyValuePairs(const std::vector<QString>& keys, const std::vector<uint32_t>& values)
    {
        if (keys.size() != values.size())
        {
            qWarning() << "Trying to create bimap from keys and values arrays of different sizes";
            return;
        }

        _kvMap.clear();
        _vkMap.clear();

        for (size_t i = 0; i < keys.size(); i++)
        {
            _kvMap[keys[i]] = values[i];
            _vkMap[values[i]] = keys[i];
        }
    }

    std::vector<QString> BiMap::getKeysByValues(const std::vector<uint32_t>& values) const
    {
        std::vector<QString> keys(values.size());

        for (size_t i = 0; i < values.size(); i++)
        {
            try
            {
                keys[i] = _vkMap.at(values[i]);
            }
            catch (std::out_of_range& oor)
            {
                qWarning() << "Trying to find value: " << values[i] << " in value-key map, but no such value exists. Error: " << oor.what();
                return std::vector<QString>();
            }
        }

        return keys;
    }

    std::vector<uint32_t> BiMap::getValuesByKeys(const std::vector<QString>& keys) const
    {
        std::vector<uint32_t> values;// (keys.size());

        //for (int i = 0; i < values.size(); i++)
        //{
        //    try
        //    {
        //        values[i] = _kvMap.at(keys[i]);
        //    }
        //    catch (std::out_of_range& oor)
        //    {
        //        qWarning() << "Trying to find key: " << keys[i] << " in key-value map, but no such key exists.";
        //        return std::vector<uint32_t>();
        //    }
        //}

        for (size_t i = 0; i < keys.size(); i++)
        {
            if (auto it = _kvMap.find(keys[i]); it != _kvMap.end()) {
                values.push_back(it->second);
            }
        }

        return values;
    }

    void BiMap::fromVariantMap(const QVariantMap& variantMap)
    {
        mv::util::variantMapMustContain(variantMap, "kvKeys");
        mv::util::variantMapMustContain(variantMap, "kvValues");

        QStringList kvMapKeys;
        QVector<uint32_t> kvMapValues;

        mv::util::loadFromDisk(variantMap["kvKeys"].toMap(), kvMapKeys);
        mv::util::loadFromDisk(variantMap["kvValues"].toMap(), kvMapValues);

        std::vector<QString> keys(kvMapKeys.size());
        std::vector<uint32_t> values(kvMapKeys.size());

        for (int i = 0; i < kvMapKeys.size(); i++)
        {
            keys[i] = kvMapKeys[i];
            values[i] = kvMapValues[i];
        }

        addKeyValuePairs(keys, values);
    }

    QVariantMap BiMap::toVariantMap() const
    {
        QVariantMap variantMap;

        QStringList kvMapKeys(_kvMap.size());
        QVector<uint32_t> kvMapValues(_kvMap.size());

        int i = 0;
        for (const auto& [key, value] : _kvMap)
        {
            kvMapKeys[i] = key;
            kvMapValues[i] = value;
            i++;
        }

        QVariantMap kvKeys = mv::util::storeOnDisk(kvMapKeys);
        QVariantMap kvValues = mv::util::storeOnDisk(kvMapValues);

        variantMap["kvKeys"] = kvKeys;
        variantMap["kvValues"] = kvValues;

        return variantMap;
    }

    void KeyBasedSelectionGroup::addDataset(Dataset<DatasetImpl> dataset, BiMap& bimap)
    {
        _datasets.push_back(dataset);
        _biMaps.push_back(std::move(bimap)); // FIXME Make it clear to caller that this happens
    }

    void KeyBasedSelectionGroup::selectionChanged(Dataset<DatasetImpl> dataset, const std::vector<uint32_t>& indices) const
    {
        if (indices.empty()) return;

        std::vector<QString> keys;

        // Find the keys associated with the given indices
        for (size_t i = 0; i < _datasets.size(); i++)
        {
            Dataset<DatasetImpl> d = _datasets[i];
            if (d == dataset)
            {
                keys = _biMaps[i].getKeysByValues(indices);
            }
        }
        // Find values associated with the found keys in other datasets
        for (size_t i = 0; i < _datasets.size(); i++)
        {
            Dataset<DatasetImpl> d = _datasets[i];
            if (d != dataset)
            {
                std::vector<uint32_t> indices = _biMaps[i].getValuesByKeys(keys);
                
                d->setSelectionIndices(indices);

                d->markSelectionDirty(true);
            }
        }
    }

    void KeyBasedSelectionGroup::fromVariantMap(const QVariantMap& variantMap)
    {
        mv::util::variantMapMustContain(variantMap, "DatasetIds");
        mv::util::variantMapMustContain(variantMap, "BiMaps");

        QStringList datasetIds;
        mv::util::loadFromDisk(variantMap["DatasetIds"].toMap(), datasetIds);
        
        for (const QString& datasetId : datasetIds)
        {
            _datasets.push_back(mv::data().getDataset(datasetId));
        }

        QVariantList bimapVariantList = variantMap["BiMaps"].value<QVariantList>();

        _biMaps.resize(bimapVariantList.size());
        for (int i = 0; i < bimapVariantList.size(); i++)
        {
            _biMaps[i].fromVariantMap(bimapVariantList[i].toMap());
        }
    }

    QVariantMap KeyBasedSelectionGroup::toVariantMap() const
    {
        QVariantMap variantMap;

        QStringList datasetIds;
        for (int i = 0; i < _datasets.size(); i++)
        {
            datasetIds.push_back(_datasets[i].getDatasetId());
        }
        QVariantMap datasetIdsMap = mv::util::storeOnDisk(datasetIds);

        QVariantList bimapVariantList;
        bimapVariantList.reserve(_biMaps.size());

        for (const BiMap& biMap : _biMaps)
        {
            bimapVariantList.push_back(biMap.toVariantMap());
        }

        variantMap.insert({
            { "DatasetIds", datasetIdsMap },
            { "BiMaps", QVariant::fromValue(bimapVariantList) }
        });

        return variantMap;
    }
}
