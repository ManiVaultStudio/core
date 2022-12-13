#include "DataManager.h"
#include "RawData.h"
#include "DataHierarchyItem.h"

#include "util/Exception.h"

#include <QRegularExpression>
#include <cassert>
#include <iostream>
#include <stdexcept>

using namespace hdps::util;

#ifdef _DEBUG
    #define DATA_MANAGER_VERBOSE
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
        // Except in the case of an invalid dataset smart pointer
        if (!dataset.isValid())
            throw std::runtime_error("Dataset smart pointer is invalid");

        // Add the data set to the map
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

        qDebug() << "Removing" << dataset->getGuiName() << "from the data manager";

        for (auto& underiveDataset : _datasets) {
            if (underiveDataset->isDerivedData() && underiveDataset->getSourceDataset<DatasetImpl>()->getGuid() == dataset->getGuid()) {
                qDebug() << "Un-derive" << underiveDataset->getGuiName();

                underiveDataset->_derived = false;
                underiveDataset->setSourceDataSet(Dataset<DatasetImpl>());
            }
        }

        _datasets.removeOne(dataset);

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
        if (datasetGuid.isEmpty())
            throw std::runtime_error("Dataset GUID is invalid");

        const auto it = std::find_if(_datasets.begin(), _datasets.end(), [datasetGuid](Dataset<DatasetImpl> dataset) -> bool {
            return datasetGuid == dataset->getGuid();
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
        variantMap[dataset->getGuid()] = dataset->toVariantMap();

    return variantMap;
}

void DataManager::reset()
{
#ifdef DATA_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif
}

void DataManager::initalize()
{
#ifdef DATA_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif
}

DataManager::~DataManager()
{
    reset();
}

} // namespace hdps
