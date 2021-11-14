#include "DataManager.h"
#include "RawData.h"
#include "DataHierarchyItem.h"

#include <QRegularExpression>
#include <cassert>
#include <iostream>

namespace hdps
{

void DataManager::addRawData(plugin::RawData* rawData)
{
    _rawDataMap.emplace(rawData->getName(), std::unique_ptr<plugin::RawData>(rawData));
}

void DataManager::addSet(DataSet& dataSet)
{
    // Add the data set to the map
    _dataSetMap.emplace(dataSet.getId(), std::unique_ptr<DataSet>(&dataSet));

    emit dataChanged();
}

void DataManager::addSelection(QString dataName, DataSet* selection)
{
    _selections.emplace(dataName, std::unique_ptr<DataSet>(selection));
}

void DataManager::removeDataset(const QString& datasetName, const bool& recursively /*= true*/)
{
    qDebug() << "Removing" << datasetName << "from the data manager";

    // Turn all derived datasets referring to the dataset to be removed to non-derived
    for (auto it = _dataSetMap.begin(); it != _dataSetMap.end();)
    {
        DataSet& set = *it->second;
        
        if (set.isDerivedData() && set.getSourceName() == datasetName)
        {
            qDebug() << "Un-derive" << set.getGuiName();

            set._derived = false;
            set._sourceSetName = "";
        }
        it++;
    }

    // Remove dataset
    _dataSetMap.erase(datasetName);
}

plugin::RawData& DataManager::getRawData(QString name)
{
    if (_rawDataMap.find(name) == _rawDataMap.end())
        throw DataNotFoundException(name);

    return *_rawDataMap[name];
}

hdps::DataSet& DataManager::getSet(const QString& datasetId)
{
    if (_dataSetMap.find(datasetId) == _dataSetMap.end())
        throw SetNotFoundException(datasetId);

    return *_dataSetMap[datasetId];
}

DataSet& DataManager::getSelection(QString name)
{
    DataSet* selection = _selections[name].get();

    if (!selection)
        throw SelectionNotFoundException(name);

    return *selection;
}

const std::unordered_map<QString, std::unique_ptr<DataSet>>& DataManager::allSets() const
{
    return _dataSetMap;
}

} // namespace hdps
