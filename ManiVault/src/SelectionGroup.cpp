#include "SelectionGroup.h"

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

    void KeyBasedSelectionGroup::addDataset(Dataset<DatasetImpl> dataset, BiMap& bimap)
    {
        _datasets.push_back(dataset);
        _biMaps.push_back(std::move(bimap));
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
}
