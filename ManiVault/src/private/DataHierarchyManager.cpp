// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DataHierarchyManager.h"
#include "DataManager.h"

#include <util/Exception.h>

#include <QtConcurrent>

#include <algorithm>
#include <stdexcept>

using namespace mv::util;

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

UniqueWorkflowPlan DataHierarchyManager::fromVariantMapWorkflow(const QVariantMap& variantMap, const SharedWorkflowExecutionContext& parentExecutionContext)
{
    UniqueWorkflowPlan plan = std::make_unique<WorkflowPlan>(__FUNCTION__);

    populateDataHierarchy(variantMap);

    std::vector<QVariantMap> datasetMaps;

    const std::function<void(const QVariantMap&)> enumerateDatasetNames = [&enumerateDatasetNames, &datasetMaps](const QVariantMap& variantMap) -> void {
        for (const auto& variant : variantMap.values()) {
            enumerateDatasetNames(variant.toMap()["Children"].toMap());

            const auto datasetMap = variant.toMap()["Dataset"].toMap();

            datasetMaps.emplace_back(datasetMap);
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

    for (const auto& dataVariantMap : datasetMaps) {
        const auto datasetId    = dataVariantMap["ID"].toString();
        const auto datasetName  = dataVariantMap["Name"].toString();

        datasetJobs.emplace_back(datasetName, [datasetId, dataVariantMap](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) {
            auto dataset    = mv::data().getDataset(datasetId);
            auto nestedPlan = dataset->fromVariantMapWorkflow(dataVariantMap,executionContext);

            WorkflowRuntimeScoped::executeBlocking(std::move(nestedPlan), executionContext);
        });
    }

    const auto idealThreads = std::max(1u, std::thread::hardware_concurrency());

    std::size_t datasetBatchSize = 8;

    if (idealThreads <= 16)
        datasetBatchSize = 2;

    if (idealThreads <= 4)
        datasetBatchSize = 1;

    plan->addBatchedParallelStage("Load datasets", std::move(datasetJobs), 2);

    plan->addSequentialStage("Notify datasets", [this](const WorkflowPlan::Job& job) {
        for (const auto& item : _items) {
            events().notifyDatasetDataChanged(item->getDataset());
        }
	});

    return plan;
}

UniqueWorkflowPlan DataHierarchyManager::toVariantMapWorkflow() const
{
    auto context    = std::make_shared<ToVariantMapWorkflowContext>();
    auto plan       = std::make_unique<WorkflowPlan>(__FUNCTION__, context);

    if (_items.empty())
        return plan;

    WorkflowPlan::Jobs saveItemsJobs;
    saveItemsJobs.reserve(_items.size());

    for (const auto& uniqueItem : _items) {
        auto* item = uniqueItem.get();

        const auto datasetId = item->getDataset()->getId();

        saveItemsJobs.emplace_back(datasetId, [context, item, datasetId](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) {
            auto itemMap = item->toVariantMapScoped(executionContext);

            itemMap.remove("Dataset");

            itemMap["Children"] = QVariantMap{};

            context->setItemMap(datasetId, itemMap);
        });
    }

    const auto idealThreads = std::max(1u, std::thread::hardware_concurrency());

    std::size_t datasetBatchSize = 8;

    if (idealThreads <= 16)
        datasetBatchSize = 2;

    if (idealThreads <= 4)
        datasetBatchSize = 1;

    plan->addSequentialStage("Save items", std::move(saveItemsJobs), datasetBatchSize);

    WorkflowPlan::Jobs saveDatasetsJobs;
    saveDatasetsJobs.reserve(_items.size());

    for (const auto& uniqueItem : _items) {
        auto* item = uniqueItem.get();

        const auto dataset      = item->getDataset();
        const auto datasetId    = dataset->getId();

        saveDatasetsJobs.emplace_back(datasetId, [context, datasetId](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) {
            auto dataset = mv::data().getDataset(datasetId);

            if (!dataset.isValid())
                return;

            auto datasetPlan    = dataset->toVariantMapWorkflow();
            auto result         = WorkflowRuntimeScoped::executeBlocking(std::move(datasetPlan), executionContext);

            const auto resultMap = result->value<QVariantMap>();

            if (resultMap.contains(datasetId)) {
                context->setDatasetMap(datasetId, resultMap[datasetId].toMap());
            } else {
                throw std::runtime_error(QString("Dataset workflow did not return expected dataset map for dataset ID %1").arg(datasetId).toStdString());
            }
        });
    }

    plan->addSequentialStage("Save datasets", std::move(saveDatasetsJobs));

    plan->addSequentialStage("Assemble hierarchy", [this, context](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&){
        std::function<QVariantMap(DataHierarchyItem*, std::int32_t)> assembleItem;

        assembleItem = [context, &assembleItem](DataHierarchyItem* item, std::int32_t sortIndex) {
            const auto datasetId    = item->getDataset()->getId();
            const auto datasetMap   = context->getDatasetMap(datasetId);

            auto itemMap = context->getItemMap(datasetId);

            Q_ASSERT(!itemMap.isEmpty());
            Q_ASSERT(!datasetMap.isEmpty());
            Q_ASSERT(datasetMap["ID"].toString() == datasetId);

            QVariantMap children;
            std::int32_t childSortIndex = 0;

            for (auto child : item->getChildren()) {
                const auto childDatasetId = child->getDataset()->getId();

                children[childDatasetId] =
                    assembleItem(child, childSortIndex++);
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

            dataHierarchyMap[datasetId] =
                assembleItem(topLevelItem, topLevelSortIndex++);
        }

        context->setDataHierarchyMap(dataHierarchyMap);
    });

    plan->addFinalizationStage("Publish result", [this, context](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext){
        executionContext->setOutput(context->getDataHierarchyMap());
    });

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
