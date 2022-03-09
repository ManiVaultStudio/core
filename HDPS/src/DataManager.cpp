#include "DataManager.h"
#include "RawData.h"
#include "DataHierarchyItem.h"

#include "util/Exception.h"

#include <QRegularExpression>
#include <cassert>
#include <iostream>
#include <stdexcept>

using namespace hdps::util;

namespace hdps
{

void DataManager::addRawData(plugin::RawData* rawData)
{
    _rawDataMap.emplace(rawData->getName(), std::unique_ptr<plugin::RawData>(rawData));
}

void DataManager::addSet(const Dataset<DatasetImpl>& dataset)
{
    try
    {
        // Except in the case of an invalid dataset smart pointer
        if (!dataset.isValid())
            throw std::runtime_error("Dataset smart pointer is invalid");

        // Add the data set to the map
        _dataSetMap.emplace(dataset->getGuid(), dataset);

        emit dataChanged();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to add dataset", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to add dataset");
    }
}

void DataManager::addSelection(const QString& dataName, Dataset<DatasetImpl> selection)
{
    _selections.emplace(dataName, selection);
}

void DataManager::removeDataset(Dataset<DatasetImpl> dataset)
{
    try
    {
        // Except when the dataset smart pointer is invalid
        if (!dataset.isValid())
            throw std::runtime_error("Dataset smart pointer is invalid");

        qDebug() << "Removing" << dataset->getGuiName() << "from the data manager";

        // Turn all derived datasets referring to the dataset to be removed to non-derived
        for (auto it = _dataSetMap.begin(); it != _dataSetMap.end();)
        {
            auto& set = *it->second;

            if (set.isDerivedData() && set.getGuid() == dataset->getGuid())
            {
                qDebug() << "Un-derive" << set.getGuiName();

                set._derived = false;
                set.setSourceDataSet(Dataset<DatasetImpl>());
            }
            it++;
        }

        // Remove dataset
        _dataSetMap.erase(dataset->getGuid());

        emit dataChanged();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to remove dataset", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to remove dataset");
    }
}

plugin::RawData& DataManager::getRawData(const QString& name)
{
    try
    {
        // Except when the dataset smart pointer is invalid
        if (name.isEmpty())
            throw std::runtime_error("Raw data name is invalid");

        // Except when the raw data is not found
        if (_rawDataMap.find(name) == _rawDataMap.end())
            throw std::runtime_error("Raw data not found");

        return *_rawDataMap[name];
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to get raw data from data manager", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to get raw data from data manager");
    }
}

Dataset<DatasetImpl> DataManager::getSet(const QString& datasetGuid)
{
    try
    {
        // Except when the dataset GUI is invalid
        if (datasetGuid.isEmpty())
            throw std::runtime_error("Dataset GUID is invalid");

        // Except when the dataset is not found
        if (_dataSetMap.find(datasetGuid) == _dataSetMap.end())
            throw std::runtime_error("Set not found");

        return _dataSetMap[datasetGuid];
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to get raw data from data manager", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to get raw data from data manager");
    }

    return Dataset<DatasetImpl>();
}

Dataset<DatasetImpl> DataManager::getSelection(const QString& dataName)
{
    try
    {
        // Except when the data name is invalid
        if (dataName.isEmpty())
            throw std::runtime_error("Data name is invalid");

        // Except when the selection set is not found
        if (_selections.find(dataName) == _selections.end())
            throw std::runtime_error("Selection set not found");

        return _selections[dataName];
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to get selection dataset from data manager", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to get selection dataset from data manager");
    }

    return Dataset<DatasetImpl>();
}

const std::unordered_map<QString, Dataset<DatasetImpl>>& DataManager::allSets() const
{
    return _dataSetMap;
}

void DataManager::fromVariantMap(const QVariantMap& variantMap)
{

}

QVariantMap DataManager::toVariantMap() const
{
    QVariantMap variantMap;

    // Save all datasets to variant map
    for (std::pair<QString, Dataset<DatasetImpl>> item : _dataSetMap)
        variantMap[item.second->getGuid()] = item.second->toVariantMap();

    return variantMap;
}

} // namespace hdps
