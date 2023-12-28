// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DataManager.h"
#include "ConfirmDatasetsRemovalDialog.h"

#include <util/Exception.h>

#include <ModalTask.h>
#include <RawData.h>
#include <DataHierarchyItem.h>

#include <cassert>
#include <iostream>
#include <stdexcept>

#include <QMainWindow>

using namespace mv::util;

#ifdef _DEBUG
    #define DATA_MANAGER_VERBOSE
#endif

namespace mv
{

using namespace plugin;

DataManager::DataManager() :
    AbstractDataManager()
{
    setObjectName("Datasets");
}

DataManager::~DataManager()
{
    reset();
}

void DataManager::addRawData(plugin::RawData* rawData)
{
    try
    {

#ifdef DATA_MANAGER_VERBOSE
        qDebug() << "Add raw data" << rawData->getName() << "to the the data manager";
#endif

        _rawDataMap.emplace(rawData->getName(), rawData);

        emit rawDataAdded(rawData);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to add raw data to the data manager", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to add raw data to the data manager");
    }
}

void DataManager::removeRawData(const QString& rawDataName)
{
    try
    {
        const auto it = _rawDataMap.find(rawDataName);

        if (it == _rawDataMap.end())
            throw std::runtime_error(QString("Raw data with name %1 not found").arg(rawDataName).toStdString());

        emit rawDataAboutToBeRemoved(_rawDataMap[rawDataName]);
        {
            plugins().destroyPlugin(_rawDataMap[rawDataName]);

            _rawDataMap.erase(it);
        }
        emit rawDataRemoved(rawDataName);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to remove raw data from data manager", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to remove raw data from data manager");
    }
}

plugin::RawData* DataManager::getRawData(const QString& rawDataName)
{
    try
    {
        if (rawDataName.isEmpty())
            throw std::runtime_error("Raw data name is invalid");

        if (_rawDataMap.find(rawDataName) == _rawDataMap.end())
            throw std::runtime_error("Raw data not found");

        return _rawDataMap[rawDataName];
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to get raw data from data manager", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to get raw data from data manager");
    }

    return {};
}

QStringList DataManager::getRawDataNames() const
{
    QStringList rawDataNames;

    for (const auto& [rawDataName, rawData] : _rawDataMap)
        rawDataNames << rawDataName;

    return rawDataNames;
}

void DataManager::addDataset(Dataset<DatasetImpl> dataset, Dataset<DatasetImpl> parentDataset, bool visible /*= true*/, bool notify /*= true*/)
{
    try
    {

#ifdef DATA_MANAGER_VERBOSE
        qDebug() << "Add dataset" << dataset->getGuiName() << "to the data manager";
#endif

        if (!dataset.isValid())
            throw std::runtime_error("Dataset smart pointer is invalid");

        _datasets.push_back(std::unique_ptr<DatasetImpl>(dataset.get()));

        if (notify)
            events().notifyDatasetAdded(dataset);

        emit datasetAdded(dataset, parentDataset, visible);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to add dataset to the data manager", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to add dataset to the data manager");
    }
}

void DataManager::removeDataset(Dataset<DatasetImpl> dataset)
{
    try
    {

#ifdef DATA_MANAGER_VERBOSE
        qDebug() << "Remove dataset" << dataset->getGuiName() << "from the data manager";
#endif

        if (!dataset.isValid())
            throw std::runtime_error("Dataset smart pointer is invalid");

        const auto datasetId        = dataset->getId();
        const auto datasetType      = dataset->getDataType();
        const auto rawDatasetName   = dataset->getRawDataName();

        dataset->setAboutToBeRemoved();

        for (const auto& underiveDataset : _datasets) {
            if (underiveDataset->isDerivedData() && underiveDataset->getSourceDataset<DatasetImpl>()->getId() == dataset->getId()) {
                if (underiveDataset->mayUnderive()) {
                    underiveDataset->_derived = false;
                    underiveDataset->setSourceDataSet(Dataset<DatasetImpl>());
                }
                else {
                    removeDataset(underiveDataset.get());
                }
            }
        }

        for (const auto dataHierarchyItem : dataset->getDataHierarchyItem().getChildren())
            if (!dataHierarchyItem->getDataset()->mayUnderive())
                removeDataset(dataHierarchyItem->getDataset());

        events().notifyDatasetAboutToBeRemoved(dataset);
        {
            emit datasetAboutToBeRemoved(dataset);
            {
                const auto it = std::find_if(_datasets.begin(), _datasets.end(), [datasetId](const auto& datasetPtr) -> bool {
                    return datasetId == datasetPtr->getId();
                });

                if (it == _datasets.end())
                    throw std::runtime_error(QString("Dataset with id %1 not found in database").arg(dataset->getId()).toStdString());

                _datasets.erase(it);
            }
            emit datasetRemoved(datasetId);
        }
        events().notifyDatasetRemoved(datasetId, datasetType);

        removeRawData(rawDatasetName);
        removeSelection(rawDatasetName);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to remove dataset from the data manager", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to remove dataset from the data manager");
    }
}

void DataManager::removeDatasets(Datasets datasets)
{
    try {
#ifdef DATA_MANAGER_VERBOSE
        qDebug() << "Remove datasets from the data manager supervised";
#endif

        if (datasets.isEmpty())
            throw std::runtime_error("No datasets to remove");

        Datasets datasetsToRemove;

        if (settings().getMiscellaneousSettings().getConfirmDatasetsRemovalAction().isChecked()) {
            ConfirmDatasetsRemovalDialog confirmDatasetsRemovalDialog(datasets);

            if (confirmDatasetsRemovalDialog.exec() == QDialog::Rejected)
                return;

            datasetsToRemove = confirmDatasetsRemovalDialog.getDatasetsToRemove();
        }
        else {
            DataHierarchyItems dataHierarchyItems;

            for (const auto& dataset : datasets)
                dataHierarchyItems << dataset->getDataHierarchyItem().getChildren();

            dataHierarchyItems.erase(std::unique(dataHierarchyItems.begin(), dataHierarchyItems.end()), dataHierarchyItems.end());

            std::sort(dataHierarchyItems.begin(), dataHierarchyItems.end(), [](auto dataHierarchyItemA, auto dataHierarchyItemB) -> bool {
                return dataHierarchyItemA->getDepth() < dataHierarchyItemB->getDepth();
            });

            std::reverse(dataHierarchyItems.begin(), dataHierarchyItems.end());

            for (const auto& dataHierarchyItem : dataHierarchyItems)
                datasetsToRemove << dataHierarchyItem->getDataset();
        }

        if (!datasetsToRemove.isEmpty()) {
            auto task = ModalTask(this, "Remove dataset(s)", Task::Status::Running);

            task.setSubtasks(datasetsToRemove.count());

            for (const auto& datasetToRemove : datasetsToRemove) {
                const auto datasetIndex     = datasetsToRemove.indexOf(datasetToRemove);
                const auto datasetGuiName   = datasetToRemove->getGuiName();

                task.setSubtaskStarted(datasetIndex, QString("Removing %1").arg(datasetGuiName));
                {
                    removeDataset(datasetToRemove);
                }
                task.setSubtaskFinished(datasetIndex, QString("Removed %1").arg(datasetGuiName));
            }

            task.setFinished();
        }

#ifdef DATA_MANAGER_VERBOSE
        qDebug() << "Raw data count:" << _rawDataMap.size();
        qDebug() << "Datasets count:" << _datasets.size();
        qDebug() << "Selection count:" << _selections.size();
#endif
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to remove dataset supervised", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to remove dataset supervised");
    }
}

Dataset<DatasetImpl> DataManager::getDataset(const QString& datasetId)
{
    try
    {
        if (datasetId.isEmpty())
            throw std::runtime_error("Dataset GUID is invalid");

        const auto it = std::find_if(_datasets.begin(), _datasets.end(), [datasetId](const auto& datasetPtr) -> bool {
            return datasetId == datasetPtr->getId();
        });

        if (it == _datasets.end())
            throw std::runtime_error(QString("Dataset with id %1 not found in database").arg(datasetId).toStdString());

        return (*it).get();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to get set from data manager", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to get set from data manager");
    }

    return {};
}

Datasets DataManager::getAllDatasets(const std::vector<DataType>& dataTypes /*= std::vector<DataType>()*/) const
{
    QVector<Dataset<DatasetImpl>> allDatasets;

    for (const auto& dataset : _datasets) {
        if (dataTypes.empty()) {
            allDatasets << dataset.get();
        }
        else {
            if (std::find(dataTypes.begin(), dataTypes.end(), dataset->getDataType()) != dataTypes.end())
                allDatasets << dataset.get();
        }
    }

    return allDatasets;
}

void DataManager::addSelection(const QString& rawDataName, Dataset<DatasetImpl> selection)
{
    _selections.push_back(std::unique_ptr<DatasetImpl>(selection.get()));

    emit selectionAdded(selection);
}

void DataManager::removeSelection(const QString& rawDataName)
{
    try
    {
        const auto it = std::find_if(_selections.begin(), _selections.end(), [rawDataName](const auto& selectionPtr) -> bool {
            return rawDataName == selectionPtr->getRawDataName();
        });

        if (it == _selections.end())
            throw std::runtime_error(QString("Selection dataset with raw data name %1 not found").arg(rawDataName).toStdString());

        const auto selection    = (*it).get();
        const auto selectionId  = selection->getId();

        emit selectionAboutToBeRemoved(selection);
        {
            _selections.erase(it);
        }
        emit selectionRemoved(selectionId);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to remove selection dataset from data manager", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to remove selection dataset from data manager");
    }
}

Dataset<DatasetImpl> DataManager::getSelection(const QString& rawDataName)
{
    try
    {
        if (rawDataName.isEmpty())
            throw std::runtime_error("Raw data name is invalid");

        const auto it = std::find_if(_selections.begin(), _selections.end(), [rawDataName](const auto& selectionPtr) -> bool {
            return rawDataName == selectionPtr->getRawDataName();
        });

        if (it == _selections.end())
            throw std::runtime_error(QString("Selection set not found for raw data %1").arg(rawDataName).toStdString());

        return (*it).get();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to get selection dataset from data manager", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to get selection dataset from data manager");
    }

    return {};
}

Datasets DataManager::getAllSelections()
{
    QVector<Dataset<DatasetImpl>> selections;

    for (const auto& dataset : _selections)
        selections << dataset.get();

    return selections;
}

void DataManager::fromVariantMap(const QVariantMap& variantMap)
{
}

QVariantMap DataManager::toVariantMap() const
{
    QVariantMap variantMap;

    for (auto& dataset : _datasets)
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
        DataHierarchyItems dataHierarchyItems;
        
        for (auto& dataset : _datasets)
            dataHierarchyItems << &dataset->getDataHierarchyItem();

        std::sort(dataHierarchyItems.begin(), dataHierarchyItems.end(), [](auto dataHierarchyItemA, auto dataHierarchyItemB) -> bool {
            return dataHierarchyItemA->getDepth() < dataHierarchyItemB->getDepth();
        });

        std::reverse(dataHierarchyItems.begin(), dataHierarchyItems.end());

        for (auto dataHierarchyItem : dataHierarchyItems)
            removeDataset(dataHierarchyItem->getDataset());
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

}
