// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DataManager.h"
#include "RawData.h"
#include "DataHierarchyItem.h"

#include "util/Exception.h"

#include <cassert>
#include <iostream>
#include <stdexcept>

using namespace hdps::util;

#ifdef _DEBUG
    //#define DATA_MANAGER_VERBOSE
#endif

namespace hdps
{

DataManager::DataManager() :
	AbstractDataManager()
{
	setObjectName("Datasets");
}

void DataManager::addRawData(plugin::RawData* rawData)
{
    _rawDataMap.emplace(rawData->getName(), std::unique_ptr<plugin::RawData>(rawData));
}

void DataManager::addSet(const Dataset<DatasetImpl>& dataset)
{
    try
    {
        if (!dataset.isValid())
            throw std::runtime_error("Dataset smart pointer is invalid");

        _datasets.push_back(dataset);

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
        if (!dataset.isValid())
            throw std::runtime_error("Dataset smart pointer is invalid");

        qDebug() << "Removing" << dataset->text() << "from the data manager";

        const auto guid = dataset->getId();
        const auto type = dataset->getDataType();

        events().notifyDatasetAboutToBeRemoved(dataset);
        {
            for (auto& underiveDataset : _datasets) {
                if (underiveDataset->isDerivedData() && underiveDataset->getSourceDataset<DatasetImpl>()->getId() == dataset->getId()) {
                    qDebug() << "Un-derive" << underiveDataset->text();

                    underiveDataset->_derived = false;
                    underiveDataset->setSourceDataSet(Dataset<DatasetImpl>());
                }
            }

            _datasets.removeOne(dataset);
        }
        events().notifyDatasetRemoved(guid, type);

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
        if (name.isEmpty())
            throw std::runtime_error("Raw data name is invalid");

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
        if (datasetGuid.isEmpty())
            throw std::runtime_error("Dataset GUID is invalid");

        const auto it = std::find_if(_datasets.begin(), _datasets.end(), [datasetGuid](Dataset<DatasetImpl> dataset) -> bool {
            return datasetGuid == dataset->getId();
        });

        if (it == _datasets.end())
            return Dataset<DatasetImpl>();

        return *it;
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to get set from data manager", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to get set from data manager");
    }

    return Dataset<DatasetImpl>();
}

Dataset<DatasetImpl> DataManager::getSelection(const QString& dataName)
{
    try
    {
        if (dataName.isEmpty())
            throw std::runtime_error("Data name is invalid");

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

const QVector<Dataset<DatasetImpl>>& DataManager::allSets() const
{
    return _datasets;
}

void DataManager::fromVariantMap(const QVariantMap& variantMap)
{
}

QVariantMap DataManager::toVariantMap() const
{
    QVariantMap variantMap;

    for (const auto& dataset : _datasets)
        variantMap[dataset->getId()] = dataset->toVariantMap();

    return variantMap;
}

void DataManager::reset()
{
#ifdef DATA_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    beginReset();
    {
        for (std::pair<QString, Dataset<DatasetImpl>> selection : _selections) {
            //delete selection.second
        }
            

        for (auto& dataset : _datasets)
            removeDataset(dataset);

        _rawDataMap.clear();

    }
    endReset();
}

void DataManager::initialize()
{
#ifdef DATA_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    AbstractDataManager::initialize();

    if (isInitialized())
        return;

    beginInitialization();
    endInitialization();
}

DataManager::~DataManager()
{
    reset();
}

}
