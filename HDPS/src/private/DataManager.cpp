// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DataManager.h"
#include "RawData.h"
#include "DataHierarchyItem.h"
#include "ConfirmDatasetsRemovalDialog.h"

#include "util/Exception.h"

#include <QMainWindow>

#include <cassert>
#include <iostream>
#include <stdexcept>

using namespace mv::util;

#ifdef _DEBUG
    //#define DATA_MANAGER_VERBOSE
#endif

namespace mv
{

DataManager::DataManager() :
    AbstractDataManager()
{
    setObjectName("Datasets");
}

void DataManager::addRawData(plugin::RawData* rawData)
{
    try
    {

#ifdef _DEBUG
        qDebug() << "Add raw data" << rawData->getName() << "to the the data manager";
#endif

        _rawDataMap.emplace(rawData->getName(), std::unique_ptr<plugin::RawData>(rawData));

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

void DataManager::addDataset(Dataset<DatasetImpl> dataset, Dataset<DatasetImpl> parentDataset, bool visible /*= true*/, bool notify /*= true*/)
{
    try
    {

#ifdef _DEBUG
        qDebug() << "Add dataset" << dataset->getGuiName() << "to the data manager";
#endif

        if (!dataset.isValid())
            throw std::runtime_error("Dataset smart pointer is invalid");

        _datasets.emplace(dataset->getId(), std::unique_ptr<DatasetImpl>(dataset.get()));

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

void DataManager::addSelection(const QString& dataName, Dataset<DatasetImpl> selection)
{
    _selections.emplace(dataName, std::unique_ptr<DatasetImpl>(selection.get()));
}

void DataManager::removeDataset(Dataset<DatasetImpl> dataset)
{
    try
    {

#ifdef _DEBUG
        qDebug() << "Remove dataset" << dataset->getGuiName() << "from the data manager";
#endif

        if (!dataset.isValid())
            throw std::runtime_error("Dataset smart pointer is invalid");

        const auto datasetId    = dataset->getId();
        const auto datasetType  = dataset->getDataType();

        dataset->setAboutToBeRemoved();

        for (auto& [datasetId, underiveDataset] : _datasets) {
            if (underiveDataset->isDerivedData() && underiveDataset->getSourceDataset<DatasetImpl>()->getId() == dataset->getId()) {
                underiveDataset->_derived = false;
                underiveDataset->setSourceDataSet(Dataset<DatasetImpl>());
            }
        }

        events().notifyDatasetAboutToBeRemoved(dataset);
        {
            emit datasetAboutToBeRemoved(dataset);

            const auto it = _datasets.find(datasetId);

            if (it == _datasets.end())
                throw std::runtime_error(QString("Dataset with id %1 not found in database").arg(dataset->getId()).toStdString());

            _datasets.erase(it);

            emit datasetRemoved(datasetId);
        }
        events().notifyDatasetRemoved(datasetId, datasetType);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to remove dataset from the data manager", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to remove dataset from the data manager");
    }
}

void DataManager::removeDatasetSupervised(Dataset<DatasetImpl> dataset)
{
    try {
#ifdef _DEBUG
        qDebug() << "Remove dataset" << dataset->text() << "from the data manager supervised";
#endif

        if (!dataset.isValid())
            throw std::runtime_error("Dataset smart pointer is invalid");

        if (settings().getMiscellaneousSettings().getConfirmDatasetsRemovalAction().isChecked()) {
            ConfirmDatasetsRemovalDialog confirmDatasetsRemovalDialog(Application::getMainWindow());

            if (confirmDatasetsRemovalDialog.exec() == QDialog::Rejected)
                return;
        }

        DataHierarchyItems dataHierarchyItems{ &dataset->getDataHierarchyItem() };

        dataHierarchyItems << dataset->getDataHierarchyItem().getChildren(true);

        std::sort(dataHierarchyItems.begin(), dataHierarchyItems.end(), [](auto dataHierarchyItemA, auto dataHierarchyItemB) -> bool {
            return dataHierarchyItemA->getDepth() < dataHierarchyItemB->getDepth();
        });

        std::reverse(dataHierarchyItems.begin(), dataHierarchyItems.end());

        for (auto dataHierarchyItem : dataHierarchyItems)
            removeDataset(dataHierarchyItem->getDataset());
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to remove dataset supervised", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to remove dataset supervised");
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

Dataset<DatasetImpl> DataManager::getSet(const QString& datasetId)
{
    try
    {
        if (datasetId.isEmpty())
            throw std::runtime_error("Dataset GUID is invalid");

        if (_datasets.find(datasetId) == _datasets.end())
            throw std::runtime_error(QString("Dataset with id %1 not found in database").arg(datasetId).toStdString());

        return _datasets[datasetId].get();
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

Dataset<DatasetImpl> DataManager::getSelection(const QString& rawDataName)
{
    try
    {
        if (rawDataName.isEmpty())
            throw std::runtime_error("Raw data name is invalid");

        if (_selections.find(rawDataName) == _selections.end())
            throw std::runtime_error(QString("Selection set not found for raw data %1").arg(rawDataName).toStdString());

        return _selections[rawDataName].get();
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

QVector<Dataset<DatasetImpl>> DataManager::allSets() const
{
    QVector<Dataset<DatasetImpl>> datasets;

    for (auto& [datasetId, dataset] : _datasets)
        datasets << dataset.get();

    return datasets;
}

void DataManager::fromVariantMap(const QVariantMap& variantMap)
{
}

QVariantMap DataManager::toVariantMap() const
{
    QVariantMap variantMap;

    for (auto& [datasetId, dataset] : _datasets)
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
        //for (std::pair<QString, Dataset<DatasetImpl>> selection : _selections) {
            //delete selection.second
        //}

        //for (auto& dataset : _datasets)
        //    removeDataset(dataset);

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
