// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DataHierarchyManager.h"
#include "DataManager.h"

#include <util/Exception.h>

#include <QMessageBox>

#include <stdexcept>

using namespace mv::util;

#ifdef _DEBUG
    //#define DATA_HIERARCHY_MANAGER_VERBOSE
#endif

namespace mv
{

DataHierarchyManager::DataHierarchyManager(QObject* parent /*= nullptr*/) :
    AbstractDataHierarchyManager(),
    _items()
{
    connect(&data(), &AbstractDataManager::datasetAboutToBeRemoved, this, &DataHierarchyManager::removeItem);
}

DataHierarchyManager::~DataHierarchyManager()
{
    reset();
}

void DataHierarchyManager::initialize()
{
#ifdef DATA_HIERARCHY_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    AbstractDataHierarchyManager::initialize();

    if (isInitialized())
        return;

    beginInitialization();
    endInitialization();
}

void DataHierarchyManager::reset()
{
#ifdef DATA_HIERARCHY_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    beginReset();
    {
    }
    endReset();
}

const DataHierarchyItem& DataHierarchyManager::getItem(const QString& datasetId) const
{
    return const_cast<DataHierarchyManager*>(this)->getItem(datasetId);
}

DataHierarchyItem& DataHierarchyManager::getItem(const QString& datasetId)
{
    try
    {
        Q_ASSERT(!datasetId.isEmpty());

        const auto it = std::find_if(_items.begin(), _items.end(), [datasetId](auto& dataHierarchyItem) -> bool {
            return datasetId == dataHierarchyItem->getDataset()->getId();
        });

        if (it == _items.end())
            throw std::runtime_error(QString("Data hierarchy item with dataset ID %1 not found in database").arg(datasetId).toStdString());

        return *(*it);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to get item from the data hierarchy manager", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to get item from the data hierarchy manager");
    }
}

mv::DataHierarchyItems DataHierarchyManager::getItems() const
{
    DataHierarchyItems items;

    for (auto& item : _items)
        if (!item->hasParent())
            items << item.get();

    return items;
}

DataHierarchyItems DataHierarchyManager::getTopLevelItems()
{
    DataHierarchyItems topLevelItems;

    for (auto& item : _items)
        if (!item->hasParent())
            topLevelItems << item.get();

    return topLevelItems;
}

mv::DataHierarchyItems DataHierarchyManager::getChildren(DataHierarchyItem& dataHierarchyItem, const bool& recursive /*= true*/)
{
    auto children = dataHierarchyItem.getChildren();

    if (recursive)
        for (auto child : children)
            children << getChildren(*child, recursive);

    return children;
}

void DataHierarchyManager::select(DataHierarchyItems items, bool clear /*= true*/)
{
    if (clear)
        clearSelection();

    for (auto& item : items)
        item->select(false);
}

void DataHierarchyManager::selectAll()
{
    for (auto& item : _items)
        item->select(false);
}

void DataHierarchyManager::clearSelection()
{
    for (auto& item : _items)
        item->deselect();
}

mv::DataHierarchyItems DataHierarchyManager::getSelectedItems() const
{
    DataHierarchyItems selectedDataHierarchyItems;

    for (auto& item : _items)
        if (item->isSelected())
            selectedDataHierarchyItems << item.get();

    return selectedDataHierarchyItems;
}

void DataHierarchyManager::addItem(Dataset<DatasetImpl> dataset, Dataset<DatasetImpl> parentDataset, const bool& visible /*= true*/)
{
    try {

#ifdef _DEBUG
        qDebug() << "Add dataset" << dataset->getGuiName() << "to the data hierarchy manager";
#endif

        const auto dataHierarchyItem = new DataHierarchyItem(dataset, parentDataset, visible);

        _items.push_back(std::unique_ptr<DataHierarchyItem>(dataHierarchyItem));

        if (parentDataset.isValid()) {
            parentDataset->getDataHierarchyItem().addChild(&dataset->getDataHierarchyItem());

            dataHierarchyItem->setParent(&parentDataset->getDataHierarchyItem());
        }

        connect(dataHierarchyItem, &DataHierarchyItem::parentChanged, this, [this, dataHierarchyItem]() -> void {
            emit itemParentChanged(*dataHierarchyItem);
        });

        emit itemAdded(*dataHierarchyItem);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to add dataset to the data hierarchy manager", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to add dataset to the data hierarchy manager");
    }
}

void DataHierarchyManager::removeItem(Dataset<DatasetImpl> dataset)
{
    try {

        if (!dataset.isValid())
            throw std::runtime_error("Dataset smart pointer is invalid");

#ifdef _DEBUG
        qDebug() << "Remove dataset" << dataset->getGuiName() << "from the data hierarchy manager";
#endif

        const auto datasetId = dataset->getId();

        const auto it = std::find_if(_items.begin(), _items.end(), [datasetId](const auto& dataHierachyItem) -> bool {
            return datasetId == dataHierachyItem->getDataset()->getId();
        });

        if (it == _items.end())
            throw std::runtime_error(QString("Data hierarchy item with dataset ID %1 not found in database").arg(datasetId).toStdString());

        emit itemAboutToBeRemoved(*(*it));
        {
            _items.erase(it);
        }
        emit itemRemoved(datasetId);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to remove item from the data hierarchy manager", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to remove item from the data hierarchy manager");
    }
}

void DataHierarchyManager::removeAllItems()
{
    _items.clear();
}

void DataHierarchyManager::fromVariantMap(const QVariantMap& variantMap)
{
    auto& projectDataSerializationTask = projects().getProjectSerializationTask().getDataTask();

    QStringList subtasks;

    const std::function<void(const QVariantMap&)> enumerateDatasetNames = [&enumerateDatasetNames, &subtasks](const QVariantMap& variantMap) -> void {
        for (const auto& variant : variantMap.values()) {
            enumerateDatasetNames(variant.toMap()["Children"].toMap());

            const auto dataset      = variant.toMap()["Dataset"].toMap();
            const auto datasetId    = dataset["ID"].toString();

            subtasks << datasetId;
        }
    };

    enumerateDatasetNames(variantMap);

    projectDataSerializationTask.setSubtasks(subtasks);
    projectDataSerializationTask.setRunning();

    const auto loadDataset = [&projectDataSerializationTask](const QVariantMap& dataHierarchyItemMap, const QString& guiName, Dataset<DatasetImpl> parent) -> Dataset<DatasetImpl> {
        const auto dataset      = dataHierarchyItemMap["Dataset"].toMap();
        const auto datasetId    = dataset["ID"].toString();
        const auto datasetName  = dataset["Name"].toString();
        const auto pluginKind   = dataset["PluginKind"].toString();

        projectDataSerializationTask.setSubtaskStarted(datasetId, QString("Loading dataset: %1").arg(datasetName));
        
        auto loadedDataset = Application::core()->addDataset(pluginKind, guiName, parent, dataset["ID"].toString());
        
        loadedDataset->getDataHierarchyItem().fromVariantMap(dataHierarchyItemMap);
        loadedDataset->fromVariantMap(dataset);

        projectDataSerializationTask.setSubtaskFinished(datasetId, QString("Loading dataset: %1").arg(datasetName));

        QCoreApplication::processEvents();

        return loadedDataset;
    };

    const std::function<void(const QVariantMap&, Dataset<DatasetImpl>)> loadDataHierarchyItem = [&loadDataHierarchyItem, loadDataset](const QVariantMap& variantMap, Dataset<DatasetImpl> parent) -> void {

        if (Application::isSerializationAborted())
            return;

        QVector<QVariantMap> sortedItems;

        sortedItems.resize(variantMap.count());

        for (const auto& variant : variantMap.values())
            sortedItems[variant.toMap()["SortIndex"].toInt()] = variant.toMap();

        for (const auto& item : sortedItems)
            loadDataHierarchyItem(item["Children"].toMap(), loadDataset(item, item["Name"].toString(), parent));
    };

    loadDataHierarchyItem(variantMap, Dataset<DatasetImpl>());

    projectDataSerializationTask.setFinished();
}

QVariantMap DataHierarchyManager::toVariantMap() const
{
    if (!_items.empty()) {
        auto& projectDataSerializationTask = projects().getProjectSerializationTask().getDataTask();
        
        QStringList subtasks;

        for (auto& dataHierarchyItem : _items)
            subtasks << dataHierarchyItem->getDataset()->getId();

        projectDataSerializationTask.setSubtasks(subtasks);
        projectDataSerializationTask.setRunning();

        QVariantMap variantMap;

        std::uint32_t sortIndex = 0;

        for (auto& dataHierarchyItem : _items) {
            if (dataHierarchyItem->hasParent())
                continue;

            const auto datasetName = dataHierarchyItem->getDataset()->getGuiName();

            projectDataSerializationTask.setSubtaskStarted(dataHierarchyItem->getDataset()->getId(), QString("Saving %1").arg(datasetName));

            auto dataHierarchyItemMap = dataHierarchyItem->toVariantMap();

            QCoreApplication::processEvents();

            projectDataSerializationTask.setSubtaskFinished(dataHierarchyItem->getDataset()->getId(), QString("Saving %1").arg(datasetName));

            dataHierarchyItemMap["SortIndex"] = sortIndex;

            variantMap[dataHierarchyItem->getDataset()->getId()] = dataHierarchyItemMap;

            sortIndex++;
        }

        projectDataSerializationTask.setFinished();

        return variantMap;
    }

    return {};
}

}
