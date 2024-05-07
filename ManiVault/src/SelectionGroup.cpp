#include "SelectionGroup.h"

#include "util/Serialization.h"

using namespace mv::util;

namespace mv
{
    BiMap::BiMap()
    {

    }

    void BiMap::addKeyValuePairs(const std::vector<QString>& keys, const std::vector<uint32_t>& values)
    {
        if (keys.size() != values.size())
        {
            qWarning() << "Trying to create bimap from keys and values arrays of different sizes";
            return;
        }

        _kvMap.clear();
        _vkMap.clear();

        for (int i = 0; i < keys.size(); i++)
        {
            _kvMap[keys[i]] = values[i];
            _vkMap[values[i]] = keys[i];
        }
    }

    std::vector<QString> BiMap::getKeysByValues(const std::vector<uint32_t>& values) const
    {
        std::vector<QString> keys(values.size());

        for (int i = 0; i < values.size(); i++)
        {
            try
            {
                keys[i] = _vkMap.at(values[i]);
            }
            catch (std::out_of_range& oor)
            {
                qWarning() << "Trying to find value: " << values[i] << " in value-key map, but no such value exists.";
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

        for (int i = 0; i < keys.size(); i++)
        {
            if (auto it = _kvMap.find(keys[i]); it != _kvMap.end()) {
                values.push_back(it->second);
            }
        }

        return values;
    }

    void BiMap::fromVariantMap(const QVariantMap& variantMap)
    {
        Serializable::fromVariantMap(variantMap);

        variantMapMustContain(variantMap, "KvKeysMap");
        variantMapMustContain(variantMap, "VkValuesMap");
        variantMapMustContain(variantMap, "KvValuesMap");
        variantMapMustContain(variantMap, "VkKeysMap");

        const auto kvKeysVariant = variantMap["KvKeysMap"].toMap();
        const auto vkValuesVariant = variantMap["VkValuesMap"].toMap();
        const auto kvValuesVariant = variantMap["KvValuesMap"].toMap();
        const auto vkKeysVariant = variantMap["VkKeysMap"].toMap();

        // Unpack the string vectors
        QStringList kvKeys;
        loadFromDisk(kvKeysVariant, kvKeys);

        QStringList vkValues;
        loadFromDisk(vkValuesVariant, vkValues);

        // Unpack the uint32_t vectors
        std::vector<uint32_t> kvValues(kvValuesVariant["Size"].value<uint64_t>() / sizeof(uint32_t));
        populateDataBufferFromVariantMap(kvValuesVariant, (char*)kvValues.data());

        std::vector<uint32_t> vkKeys(vkKeysVariant["Size"].value<uint64_t>() / sizeof(uint32_t));
        populateDataBufferFromVariantMap(vkKeysVariant, (char*)vkKeys.data());

        // Fill the unordered maps
        for (int i = 0; i < kvKeys.size(); i++)
        {
            _kvMap[kvKeys[i]] = kvValues[i];
        }

        for (int i = 0; i < vkKeys.size(); i++)
        {
            _vkMap[vkKeys[i]] = vkValues[i];
        }
    }

    QVariantMap BiMap::toVariantMap() const
    {
        auto variantMap = Serializable::toVariantMap();

        QList<QString> kvKeys;
        std::vector<uint32_t> kvValues;

        std::vector<uint32_t> vkKeys;
        QList<QString> vkValues;

        for (auto kv : _kvMap)
        {
            kvKeys.push_back(kv.first);
            kvValues.push_back(kv.second);
        }

        for (auto vk : _vkMap)
        {
            vkKeys.push_back(vk.first);
            vkValues.push_back(vk.second);
        }

        QVariantMap kvKeysVariant = storeOnDisk(kvKeys);
        QVariantMap vkValuesVariant = storeOnDisk(vkValues);

        QVariantMap kvValuesVariant = rawDataToVariantMap((const char*)kvValues.data(), kvValues.size() * sizeof(uint32_t), true);
        QVariantMap vkKeysVariant = rawDataToVariantMap((const char*)vkKeys.data(), vkKeys.size() * sizeof(uint32_t), true);

        variantMap.insert({
            { "KvKeysMap", QVariant::fromValue(kvKeysVariant) },
            { "VkValuesMap", QVariant::fromValue(vkValuesVariant) },
            { "KvValuesMap", QVariant::fromValue(kvValuesVariant) },
            { "VkKeysMap", QVariant::fromValue(vkKeysVariant) }
        });

        return variantMap;
    }

    void KeyBasedSelectionGroup::addDataset(Dataset<DatasetImpl> dataset, BiMap& bimap)
    {
        _datasets.push_back(dataset);
        _biMaps.push_back(std::move(bimap));
    }

    void KeyBasedSelectionGroup::selectionChanged(Dataset<DatasetImpl> dataset, const std::vector<uint32_t>& indices) const
    {
        std::vector<QString> keys;

        // Find the keys associated with the given indices
        for (int i = 0; i < _datasets.size(); i++)
        {
            Dataset<DatasetImpl> d = _datasets[i];
            if (d == dataset)
            {
                keys = _biMaps[i].getKeysByValues(indices);
            }
        }

        // Find values associated with the found keys in other datasets
        for (int i = 0; i < _datasets.size(); i++)
        {
            Dataset<DatasetImpl> d = _datasets[i];

            if (d != dataset)
            {
                std::vector<uint32_t> indices = _biMaps[i].getValuesByKeys(keys);

                d->setSelectionIndices(indices);

                d->dirtySelection();
            }
        }
    }

    void KeyBasedSelectionGroup::fromVariantMap(const QVariantMap& variantMap)
    {
        Serializable::fromVariantMap(variantMap);

        variantMapMustContain(variantMap, "ListOfDatasets");
        variantMapMustContain(variantMap, "ListOfBiMaps");

        const auto listOfDatasets = variantMap["ListOfDatasets"].toList();
        const auto listOfBiMaps = variantMap["ListOfBiMaps"].toList();

        for (int i = 0; i < listOfDatasets.size(); i++)
        {
            QString datasetId = listOfDatasets[i].toString();

            _datasets.push_back(mv::data().getDataset(datasetId));
        }

        for (int i = 0; i < listOfBiMaps.size(); i++)
        {
            QVariantMap biMapVariantMap = listOfBiMaps[i].toMap();

            BiMap biMap;
            biMap.fromVariantMap(biMapVariantMap);
            _biMaps.push_back(biMap);
        }
    }

    QVariantMap KeyBasedSelectionGroup::toVariantMap() const
    {
        auto variantMap = Serializable::toVariantMap();

        QVariantList listOfDatasets;
        for (int i = 0; i < _datasets.size(); i++)
        {
            listOfDatasets.push_back(_datasets[i].getDatasetId());
        }

        QVariantList listOfBiMaps;
        for (int i = 0; i < _biMaps.size(); i++)
        {
            qDebug() << _datasets[i]->getGuiName();
            listOfBiMaps.push_back(_biMaps[i].toVariantMap());
        }

        variantMap.insert({
            { "ListOfDatasets", listOfDatasets },
            { "ListOfBiMaps", listOfBiMaps }
        });

        return variantMap;
    }
}
