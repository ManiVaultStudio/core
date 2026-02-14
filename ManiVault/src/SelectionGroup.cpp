#include "SelectionGroup.h"

#include "util/Serialization.h"

#include "Set.h"

#include <QStringList>
#include <QVector>

#include <stdexcept>

namespace mv
{
    void BiMap::addKeyValuePairs(const std::vector<QString>& keys, const std::vector<uint32_t>& values)
    {
        if (keys.size() != values.size())
        {
            qWarning() << "[SelectionGroup] Trying to create bimap from keys and values arrays of different sizes";
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
        std::vector<QString> keys;
        keys.reserve(values.size());

        int invalidValueCount = 0;
        std::vector<uint32_t> missingValues;
        missingValues.reserve(3);

        for (size_t i = 0; i < values.size(); i++)
        {
            // If the value is not in the map, count up how many times this happens, and store a few to report back
            if (_vkMap.find(values[i]) == _vkMap.end())
            {
                // Store several invalid values to report back to user
                if (invalidValueCount < 3)
                    missingValues.push_back(values[i]);

                invalidValueCount++;
                continue;
            }

            keys.push_back(_vkMap.at(values[i]));
        }

        // If missing values were tried, report on them
        if (invalidValueCount > 0)
        {
            QDebug deb = qWarning();
            deb << "[SelectionGroup] Tried to find " << invalidValueCount << " values that did not exist in the vkMap.";
            deb << "Example missing values:";
            for (uint32_t val : missingValues)
            {
                deb << val << ",";
            }
        }

        return keys;
    }

    std::vector<uint32_t> BiMap::getValuesByKeys(const std::vector<QString>& keys, bool verbose) const
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
            else
            {
                if (verbose)
                    qDebug() << "This key wasn't found: " << keys[i];
            }
        }

        return values;
    }

    std::vector<int> BiMap::getValuesByKeysWithMissingValue(const std::vector<QString>& keys, int missingValue) const
    {
        std::vector<int> values;

        for (size_t i = 0; i < keys.size(); i++)
        {
            if (auto it = _kvMap.find(keys[i]); it != _kvMap.end()) {
                values.push_back(it->second);
            }
            else
            {
                values.push_back(missingValue);
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

    BiMap& KeyBasedSelectionGroup::getBiMap(Dataset<DatasetImpl> dataset)
    {
        for (size_t i = 0; i < _datasets.size(); i++)
        {
            Dataset<DatasetImpl> d = _datasets[i];
            if (d == dataset)
            {
                return _biMaps[i];
            }
        }
        throw std::runtime_error("Dataset not found in selection group");
    }

    void KeyBasedSelectionGroup::addDataset(Dataset<DatasetImpl> dataset, const std::vector<QString>& keys)
    {
        _datasets.push_back(dataset);
        BiMap bimap;
        std::vector<uint32_t> indices(keys.size());
        std::iota(indices.begin(), indices.end(), 0);
        bimap.addKeyValuePairs(keys, indices);
        _biMaps.push_back(std::move(bimap));
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
