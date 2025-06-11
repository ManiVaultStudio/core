// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DataHierarchyManager.h"
#include "DataManager.h"

#include <util/Exception.h>

#include <algorithm>
#include <stdexcept>

using namespace mv::util;

#ifdef _DEBUG
    //#define DATA_HIERARCHY_MANAGER_VERBOSE
#endif

namespace mv
{

DataHierarchyManager::DataHierarchyManager(QObject* parent) :
    AbstractDataHierarchyManager(parent)
{
    connect(core(), &CoreInterface::aboutToBeDestroyed, this, [this]() -> void {
        for (auto& item : _items)
            item->setParent(nullptr); // Clear parent to avoid dangling pointers
    });
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
    {
        connect(&data(), &AbstractDataManager::datasetAboutToBeRemoved, this, &DataHierarchyManager::removeItem);

        const auto synchronizeSelection = [this]() -> void {
            emit selectedItemsChanged(getSelectedItems());
        };

        connect(&projects(), &AbstractProjectManager::projectOpened, this, synchronizeSelection);
        connect(&projects(), &AbstractProjectManager::projectImported, this, synchronizeSelection);
    }
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

const DataHierarchyItem* DataHierarchyManager::getItem(const QString& datasetId) const
{
    return const_cast<DataHierarchyManager*>(this)->getItem(datasetId);
}

DataHierarchyItem* DataHierarchyManager::getItem(const QString& datasetId)
{
    try
    {
        Q_ASSERT(!datasetId.isEmpty());

        const auto it = std::find_if(_items.begin(), _items.end(), [datasetId](auto& dataHierarchyItem) -> bool {
            return datasetId == dataHierarchyItem->getDataset()->getId();
        });

        if (it == _items.end())
            throw std::runtime_error(QString("Data hierarchy item with dataset ID %1 not found in database").arg(datasetId).toStdString());

        return (*it).get();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to get item from the data hierarchy manager", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to get item from the data hierarchy manager");
    }

    return nullptr;
}

mv::DataHierarchyItems DataHierarchyManager::getItems() const
{
    DataHierarchyItems items;

    for (auto& item : _items)
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

DataHierarchyItems DataHierarchyManager::getChildren(DataHierarchyItem& dataHierarchyItem, const bool& recursive /*= true*/)
{
    auto children = dataHierarchyItem.getChildren();

    if (recursive)
        for (auto child : children)
            children << getChildren(*child, recursive);

    return DataHierarchyItems(children.begin(), children.end());
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

#ifdef DATA_HIERARCHY_MANAGER_VERBOSE
        qDebug() << "Add dataset" << dataset->getGuiName() << "to the data hierarchy manager";
#endif

        const auto dataHierarchyItem = new DataHierarchyItem(dataset, parentDataset, visible);

        _items.push_back(std::unique_ptr<DataHierarchyItem>(dataHierarchyItem));

        connect(dataHierarchyItem, &DataHierarchyItem::parentChanged, this, [this, dataHierarchyItem]() -> void {
            emit itemParentChanged(dataHierarchyItem);
        });

        connect(dataHierarchyItem, &DataHierarchyItem::selectedChanged, this, [this, dataHierarchyItem]() -> void {
            emit selectedItemsChanged(getSelectedItems());
        });

        emit itemAdded(dataHierarchyItem);
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

#ifdef DATA_HIERARCHY_MANAGER_VERBOSE
        qDebug() << "Remove dataset" << dataset->getGuiName() << "from the data hierarchy manager";
#endif

        const auto datasetId = dataset->getId();

        const auto it = std::find_if(_items.begin(), _items.end(), [datasetId](const auto& dataHierachyItem) -> bool {
            return datasetId == dataHierachyItem->getDataset()->getId();
        });

        if (it == _items.end())
            throw std::runtime_error(QString("Data hierarchy item with dataset ID %1 not found in database").arg(datasetId).toStdString());

        emit itemAboutToBeRemoved((*it).get());
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
    std::vector<std::pair<QVariantMap, bool>> datasetList;

    const std::function<void(const QVariantMap&)> enumerateDatasetNames = [&enumerateDatasetNames, &subtasks, &datasetList](const QVariantMap& variantMap) -> void {
        for (const auto& variant : variantMap.values()) {
            enumerateDatasetNames(variant.toMap()["Children"].toMap());

            const auto dataset      = variant.toMap()["Dataset"].toMap();
            const auto datasetId    = dataset["ID"].toString();

            subtasks << datasetId;
            datasetList.emplace_back(dataset, dataset["Derived"].toBool() || !dataset["ProxyMembers"].toStringList().isEmpty());
        }
    };

    enumerateDatasetNames(variantMap);

    projectDataSerializationTask.setSubtasks(subtasks);
    projectDataSerializationTask.setRunning();

    const auto loadDataHierarchyItem = [&projectDataSerializationTask](const QVariantMap& dataHierarchyItemMap, const QString& guiName, Dataset<DatasetImpl> parent) -> Dataset<DatasetImpl> {
        const auto dataset          = dataHierarchyItemMap["Dataset"].toMap();
        const auto datasetId        = dataset["ID"].toString();
        const auto datasetName      = dataset["Name"].toString();
        const auto pluginKind       = dataset["PluginKind"].toString();
        const auto isDerived        = dataset["Derived"].toBool();
        const auto isFull           = dataset["Full"].toBool();
        const auto sourceDatasetID  = dataset["SourceDatasetID"].toString();

        auto subtaskName = QString("Loading dataset hierarchy item: %1").arg(datasetName);
        projectDataSerializationTask.setSubtaskStarted(datasetId, subtaskName);

        Dataset<DatasetImpl> loadedDataset;

        loadedDataset = (isDerived || !isFull) ? mv::data().createDatasetWithoutSelection(pluginKind, guiName, parent, datasetId) : mv::data().createDataset(pluginKind, guiName, parent, datasetId);

        if (isDerived)
            loadedDataset->setSourceDataset(sourceDatasetID);

        loadedDataset->getDataHierarchyItem().fromVariantMap(dataHierarchyItemMap);

        projectDataSerializationTask.setSubtaskFinished(datasetId, subtaskName);

        QCoreApplication::processEvents();

        return loadedDataset;
    };

    const std::function<void(const QVariantMap&, Dataset<DatasetImpl>)> populateDataHierarchy = [&populateDataHierarchy, loadDataHierarchyItem](const QVariantMap& variantMap, Dataset<DatasetImpl> parent) -> void {

        if (Application::isSerializationAborted())
            return;

        QVector<QVariantMap> sortedItems;

        sortedItems.resize(variantMap.count());

        for (const auto& variant : variantMap.values())
            sortedItems[variant.toMap()["SortIndex"].toInt()] = variant.toMap();

        for (const auto& item : sortedItems)
            populateDataHierarchy(item["Children"].toMap(), loadDataHierarchyItem(item, item["Name"].toString(), parent));
    };

    auto populateDatasets = [&projectDataSerializationTask, &datasetList](const QVariantMap& variantMap) -> void {

        if (Application::isSerializationAborted())
            return;

        // Maintain data hierarchy item order within partitions
        std::reverse(datasetList.begin(), datasetList.end());

        // First load non-derived datasets
        std::stable_partition(datasetList.begin(), datasetList.end(),
            [](const std::pair<QVariantMap, bool>& element) {
                return !element.second; 
            });

        for (const auto& [dataVariantMap, isDerived] : datasetList)
        {
            const auto datasetId = dataVariantMap["ID"].toString();
            const auto datasetName = dataVariantMap["Name"].toString();

            auto subtaskName = QString("Loading dataset: %1").arg(datasetName);
            projectDataSerializationTask.setSubtaskStarted(datasetId, subtaskName);

            mv::data().getDataset(datasetId)->fromVariantMap(dataVariantMap);

            projectDataSerializationTask.setSubtaskFinished(datasetId, subtaskName);

            QCoreApplication::processEvents();
        }
    };

    populateDataHierarchy(variantMap, Dataset<DatasetImpl>());

    populateDatasets(variantMap);

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
