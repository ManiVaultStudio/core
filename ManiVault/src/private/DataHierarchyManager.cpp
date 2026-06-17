// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DataHierarchyManager.h"
#include "DataManager.h"

#include <util/Exception.h>
#include <util/Serialization.h>

#include <QtConcurrent>

#include <algorithm>
#include <stdexcept>

using namespace mv::util;
using namespace mv::workflow;

#ifdef _DEBUG
    //#define DATA_HIERARCHY_MANAGER_VERBOSE
#endif

namespace mv
{

namespace 
{
    /** Thread-safe data hierarchy save context */
    class ToVariantMapWorkflowContext : public WorkflowContextBase
    {
    public:
        void setItemMap(const QString& datasetId, const QVariantMap& itemMap)
        {
            QMutexLocker lock(&_mutex);
            _itemMaps[datasetId] = itemMap;
        }

        QVariantMap getItemMap(const QString& datasetId) const
        {
            QMutexLocker lock(&_mutex);

            const auto it = _itemMaps.find(datasetId);

            if (it != _itemMaps.end())
                return it.value().toMap();

            return {};
        }

        QStringList getItemIds() const
        {
            QMutexLocker lock(&_mutex);
            return _itemMaps.keys();
        }

        void setDatasetMap(const QString& datasetId, const QVariantMap& datasetMap)
        {
            QMutexLocker lock(&_mutex);
            _datasetMaps[datasetId] = datasetMap;
        }

        QVariantMap getDatasetMap(const QString& datasetId) const
        {
            QMutexLocker lock(&_mutex);

            const auto it = _datasetMaps.find(datasetId);

            if (it != _datasetMaps.end())
                return it.value().toMap();

            return {};
        }

        QStringList getDatasetIds() const
        {
            QMutexLocker lock(&_mutex);
            return _datasetMaps.keys();
        }

        void setDataHierarchyMap(const QVariantMap& dataHierarchyMap)
        {
            QMutexLocker lock(&_mutex);
            _dataHierarchyMap = dataHierarchyMap;
        }

        QVariantMap getDataHierarchyMap() const
        {
            QMutexLocker lock(&_mutex);
            return _dataHierarchyMap;
        }

    private:
        mutable QMutex _mutex;

        QVariantMap _itemMaps;
        QVariantMap _datasetMaps;
        QVariantMap _dataHierarchyMap;
    };

}

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

UniqueWorkflowPlan DataHierarchyManager::fromVariantMapWorkflow(QVariantMap variantMap)
{
    UniqueWorkflowPlan plan = std::make_unique<WorkflowPlan>(__FUNCTION__);

    plan->addSequentialStage("Populate", [this, variantMap](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&) {
        populateDataHierarchy(variantMap);
    }, WorkflowPlan::JobThreadAffinity::GuiThread, 1.0);

    std::vector<QVariantMap> datasetMaps;
    std::map<QString, std::uint64_t> approximateDatasetSizes;

    const std::function<void(const QVariantMap&)> enumerateDatasetNames = [&enumerateDatasetNames, &datasetMaps, &approximateDatasetSizes](const QVariantMap& variantMap) -> void {
        for (const auto& variant : variantMap.values()) {
            enumerateDatasetNames(variant.toMap()["Children"].toMap());

            auto datasetMap = variant.toMap()["Dataset"].toMap();

            datasetMap.remove("Children");

            datasetMaps.emplace_back(datasetMap);

            approximateDatasetSizes[datasetMap["ID"].toString()] = getRawBlockObjectSize(datasetMap);
        }
    };

    enumerateDatasetNames(variantMap);

    std::sort(datasetMaps.begin(), datasetMaps.end(), [](const auto& a, const auto& b) {
        const auto rawA = findRawBlockObject(a);
        const auto rawB = findRawBlockObject(b);
        const auto hasA = !rawA.isEmpty();
        const auto hasB = !rawB.isEmpty();

        if (hasA != hasB)
            return hasA;

        if (!hasA)
            return false;

        return getRawBlockObjectSize(rawA) > getRawBlockObjectSize(rawB);
    });

    WorkflowPlan::Jobs datasetJobs;
    datasetJobs.reserve(datasetMaps.size());

    for (const auto& dataVariantMap : datasetMaps) {
        const auto datasetId = dataVariantMap["ID"].toString();
        const auto datasetName = dataVariantMap["Name"].toString();

        datasetJobs.emplace_back(QString("Load %1").arg(datasetName), WorkflowPlan::NestedWorkflowFunction([datasetId, dataVariantMap, approximateDatasetSizes](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&) -> UniqueWorkflowPlan {
            auto dataset = mv::data().getDataset(datasetId);

            Q_ASSERT(dataset.isValid());

            return dataset->fromVariantMapWorkflow(dataVariantMap);
        }), WorkflowPlan::JobThreadAffinity::GuiThread, WorkflowPlan::JobProgressMode::Automatic, approximateDatasetSizes[datasetId]);
    }

    plan->addBatchedParallelStage("Load datasets", std::move(datasetJobs), [](const SharedWorkflowExecutionContext& executionContext) {
        return executionContext->getState()
            ->getExecutionOptions()
            ._workflowBatchingOptions
            ._datasetLoadingBatchSize;
    });

    plan->addSequentialStage("Notify datasets", [this](const WorkflowPlan::Job& job, const SharedWorkflowExecutionContext&) {
        for (const auto& item : _items) {
            events().notifyDatasetDataChanged(item->getDataset());
        }
	});

    return plan;
}

UniqueWorkflowPlan DataHierarchyManager::toVariantMapWorkflow() const
{
    auto context = std::make_shared<ToVariantMapWorkflowContext>();

    auto plan = std::make_unique<WorkflowPlan>(__FUNCTION__);

    if (_items.empty())
        return plan;

    const auto saveItemMapsStage = plan->addSequentialStage("Save items", [this](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) {
        QVariantMap itemMaps;

        for (const auto& uniqueItem : _items) {
            auto* item = uniqueItem.get();

            const auto datasetId = item->getDataset()->getId();
            auto itemMap = item->toVariantMap();

            itemMap.remove("Dataset");

            itemMap["Children"] = QVariantMap{};

            itemMaps[datasetId] = itemMap;
        }

        executionContext->setOutput(itemMaps);
    }, WorkflowPlan::JobThreadAffinity::GuiThread, 1.0);

    WorkflowPlan::Jobs saveDatasetsJobs;
    saveDatasetsJobs.reserve(_items.size());

    QHash<QString, WorkflowHandle> datasetHandles;

    for (const auto& uniqueItem : _items) {
        auto* item = uniqueItem.get();

        const auto dataset = item->getDataset();
        const auto datasetId = dataset->getId();

        WorkflowPlan::Job job(QString("Save dataset %1").arg(datasetId), WorkflowPlan::NestedWorkflowFunction([datasetId](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&) -> UniqueWorkflowPlan {
            auto dataset = mv::data().getDataset(datasetId);

            if (!dataset.isValid())
                return {};

            return dataset->toVariantMapWorkflow();
        }), WorkflowPlan::JobThreadAffinity::CurrentWorkerThread, WorkflowPlan::JobProgressMode::Nested);

        datasetHandles.insert(datasetId, job.getHandle());

        saveDatasetsJobs.emplace_back(std::move(job));
    }

    plan->addBatchedParallelStage("Save datasets", std::move(saveDatasetsJobs), [](const SharedWorkflowExecutionContext& executionContext) {
        return executionContext->getState()
            ->getExecutionOptions()
            ._workflowBatchingOptions
            ._datasetLoadingBatchSize;
    });

    qDebug() << datasetHandles.keys();
    const auto collectDatasetMapsStage = plan->addSequentialStage("Collect dataset maps", [saveItemMapsStage, datasetHandles](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) {
        const auto itemMaps = executionContext->takeOutput(saveItemMapsStage).toMap();

        QVariantMap datasetMaps;

        for (auto it = datasetHandles.constBegin(); it != datasetHandles.constEnd(); ++it) {
            const auto datasetId    = it.key();
            const auto handle       = it.value();
            const auto output       = executionContext->takeOutput(handle);
            const auto datasetMap   = output.toMap();

            datasetMaps.insert(datasetId, datasetMap);
        }

        executionContext->setOutput(QVariantMap{
            { "Items", itemMaps },
            { "Datasets", datasetMaps }
        });
    }, WorkflowPlan::JobThreadAffinity::GuiThread, 1.0);
    
    plan->addSequentialStage("Assemble hierarchy", [this, collectDatasetMapsStage](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext){
        const auto itemsAndDatasetsMap = executionContext->takeOutput(collectDatasetMapsStage).toMap();

        std::function<QVariantMap(DataHierarchyItem*, std::int32_t)> assembleItem;

        assembleItem = [&assembleItem, &itemsAndDatasetsMap](DataHierarchyItem* item, std::int32_t sortIndex) {
            const auto datasetId    = item->getDataset()->getId();
            const auto datasetMap   = itemsAndDatasetsMap["Datasets"].toMap()[datasetId].toMap();

            auto itemMap = itemsAndDatasetsMap["Items"].toMap()[datasetId].toMap();

            Q_ASSERT(!itemMap.isEmpty());
            Q_ASSERT(!datasetMap.isEmpty());
            Q_ASSERT(datasetMap["ID"].toString() == datasetId);

            QVariantMap children;
            std::int32_t childSortIndex = 0;

            for (auto child : item->getChildren()) {
                const auto childDatasetId = child->getDataset()->getId();

                children[childDatasetId] = assembleItem(child, childSortIndex++);
            }

            itemMap["Dataset"]      = datasetMap;
            itemMap["Children"]     = children;
            itemMap["SortIndex"]    = sortIndex;

            return itemMap;
        };

        QVariantMap dataHierarchyMap;
        std::int32_t topLevelSortIndex = 0;

        for (auto topLevelItem : const_cast<DataHierarchyManager*>(this)->getTopLevelItems()) {
            const auto datasetId = topLevelItem->getDataset()->getId();

            dataHierarchyMap[datasetId] = assembleItem(topLevelItem, topLevelSortIndex++);
        }

        executionContext->setOutput(dataHierarchyMap);
    }, WorkflowPlan::JobThreadAffinity::GuiThread, 1.0);

    return plan;
}

QVector<QVariantMap> DataHierarchyManager::sortedDataHierarchyItems(const QVariantMap& itemsMap)
{
    QVector<QVariantMap> items;
    items.reserve(itemsMap.size());

    for (const auto& item : itemsMap)
        items << item.toMap();

    std::ranges::sort(items, {}, [](const QVariantMap& item) {
        return item["SortIndex"].toInt();
        });

    return items;
}

void DataHierarchyManager::populateDataHierarchy(const QVariantMap& itemsMap, const Dataset<DatasetImpl>& parent)
{
    for (const auto& itemMap : sortedDataHierarchyItems(itemsMap)) {
        auto dataset = createDatasetFromItem(itemMap, parent);
        populateDataHierarchy(itemMap["Children"].toMap(), dataset);
    }
}

Dataset<DatasetImpl> DataHierarchyManager::createDatasetFromItem(const QVariantMap& itemMap, const Dataset<DatasetImpl>& parent)
{
    const auto datasetMap       = itemMap["Dataset"].toMap();
    const auto datasetId        = datasetMap["ID"].toString();
    const auto guiName          = datasetMap["Name"].toString();
    const auto pluginKind       = datasetMap["PluginKind"].toString();
    const auto isDerived        = datasetMap["Derived"].toBool();
    const auto isFull           = datasetMap["Full"].toBool();
    const auto sourceDatasetId  = datasetMap["SourceDatasetID"].toString();

    auto dataset = isDerived || !isFull ? mv::data().createDatasetWithoutSelection(pluginKind, guiName, parent, datasetId) : mv::data().createDataset(pluginKind, guiName, parent, datasetId);

    if (isDerived && !sourceDatasetId.isEmpty())
        dataset->setSourceDataset(sourceDatasetId);

    dataset->getDataHierarchyItem().fromVariantMap(itemMap);

    return dataset;
}

}
