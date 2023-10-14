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

void DataHierarchyManager::addItem(Dataset<DatasetImpl> dataset, Dataset<DatasetImpl> parentDataset, const bool& visible /*= true*/)
{
    //qDebug() << "Adding" << dataset->getGuiName() << "to the data hierarchy";

    try {

        // Create new data hierarchy item
        const auto newDataHierarchyItem = new DataHierarchyItem(parentDataset.isValid() ? &parentDataset->getDataHierarchyItem() : static_cast<QObject*>(this), dataset, parentDataset, visible);

        // Pass-through loading signal
        connect(newDataHierarchyItem, &DataHierarchyItem::loading, this, [this, newDataHierarchyItem]() {
            emit itemLoading(*newDataHierarchyItem);
        });

        // Pass-through loaded signal
        connect(newDataHierarchyItem, &DataHierarchyItem::loaded, this, [this, newDataHierarchyItem]() {
            emit itemLoaded(*newDataHierarchyItem);
        });

        // Pass-through saving signal
        connect(newDataHierarchyItem, &DataHierarchyItem::saving, this, [this, newDataHierarchyItem]() {
            emit itemSaving(*newDataHierarchyItem);
        });

        // Pass-through saved signal
        connect(newDataHierarchyItem, &DataHierarchyItem::saved, this, [this, newDataHierarchyItem]() {
            emit itemSaved(*newDataHierarchyItem);
        });

        _items << newDataHierarchyItem;

        // Add child item if the parent is valid
        if (parentDataset.isValid())
            parentDataset->getDataHierarchyItem().addChild(dataset->getDataHierarchyItem());

        dataset->setParent(newDataHierarchyItem);

        // Notify others that an item is added
        emit itemAdded(*newDataHierarchyItem);

        // Remove the data hierarchy item when the corresponding dataset is about to be removed
        connect(&newDataHierarchyItem->getDatasetReference(), &Dataset<DatasetImpl>::aboutToBeRemoved, this, [this, newDataHierarchyItem]() {
            if (newDataHierarchyItem->getDatasetReference().isValid())
                removeItem(*newDataHierarchyItem);
        });
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to add dataset to the data hierarchy manager", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to add dataset to the data hierarchy manager");
    }
}

void DataHierarchyManager::removeItem(DataHierarchyItem& dataHierarchyItem)
{
    try {
        qDebug() << "Removing" << dataHierarchyItem.getDataset()->text() << "from the data hierarchy";

        // Get dataset
        auto dataset        = dataHierarchyItem.getDataset();
        auto datasetGuid    = dataset->getId();

        emit itemAboutToBeRemoved(dataset);
        {
            _items.removeOne(&dataHierarchyItem);

            //delete &dataHierarchyItem;
        }
        emit itemRemoved(datasetGuid);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to remove item(s) from the data hierarchy manager", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to remove item(s) from the data hierarchy manager");
    }
}

void DataHierarchyManager::removeAllItems()
{
    const auto dataHierarchyItems = _items;

    for (auto dataHierarchyItem : dataHierarchyItems)
        if (!dataHierarchyItem->hasParent())
            removeItem(*dataHierarchyItem);
}

const DataHierarchyItem& DataHierarchyManager::getItem(const QString& datasetGuid) const
{
    return const_cast<DataHierarchyManager*>(this)->getItem(datasetGuid);
}

DataHierarchyItem& DataHierarchyManager::getItem(const QString& datasetGuid)
{
    try
    {
        Q_ASSERT(!datasetGuid.isEmpty());

        for (auto dataHierarchyItem : _items)
            if (dataHierarchyItem->getDatasetReference().getDatasetId() == datasetGuid)
                return *dataHierarchyItem;

        throw std::runtime_error(QString("Failed to find data hierarchy item with dataset ID: %1").arg(datasetGuid).toStdString());
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to get item from the data hierarchy manager", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to get item from the data hierarchy manager");
    }
}

mv::DataHierarchyItems DataHierarchyManager::getChildren(DataHierarchyItem& dataHierarchyItem, const bool& recursive /*= true*/)
{
    auto children = dataHierarchyItem.getChildren();

    if (recursive)
        for (auto child : children)
            children << getChildren(*child, recursive);

    return children;
}

DataHierarchyItems DataHierarchyManager::getTopLevelItems()
{
    DataHierarchyItems topLevelItems;

    for (auto item : _items)
        if (!item->hasParent())
            topLevelItems << item;

    return topLevelItems;
}

void DataHierarchyManager::selectItems(DataHierarchyItems& selectedItems)
{
    emit selectedItemsChanged(selectedItems);
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
        
        events().notifyDatasetAdded(loadedDataset);
        
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
    if (!_items.isEmpty()) {
        auto& projectDataSerializationTask = projects().getProjectSerializationTask().getDataTask();
        
        QStringList subtasks;

        for (auto dataHierarchyItem : _items)
            subtasks << dataHierarchyItem->getDataset()->getId();

        projectDataSerializationTask.setSubtasks(subtasks);
        projectDataSerializationTask.setRunning();

        QVariantMap variantMap;

        std::uint32_t sortIndex = 0;

        for (auto dataHierarchyItem : _items) {
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
