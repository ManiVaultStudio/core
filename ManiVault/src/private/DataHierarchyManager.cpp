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

        /**
         * @brief Set dataset map for dataset ID in the context
         * @param datasetId Dataset ID
         * @param datasetMap Dataset map to set for the dataset ID
         */
        void setDataset(const QString& datasetId, const QVariantMap& datasetMap)
        {
            QMutexLocker lock(&_mutex);

            _datasetsMap[datasetId] = datasetMap;
        }

        /**
         * @brief Get dataset map for dataset ID from the context
         * @param datasetId Dataset ID
         * @return Dataset map for the dataset ID, or empty map if not found
         */
        QVariantMap getDataset(const QString& datasetId) const
        {
            QMutexLocker lock(&_mutex);

            const auto it = _datasetsMap.find(datasetId);

            if (it != _datasetsMap.end())
                return it.value().toMap();

            return {};
        }

        /**
         * @brief Set data hierarchy map in the context
         * @param dataHierarchyMap Data hierarchy map to set
         */
        void setDataHierarchyMap(const QVariantMap& dataHierarchyMap)
        {
            QMutexLocker lock(&_mutex);
            _dataHierarchyMap = dataHierarchyMap;
        }

    private:
        mutable QMutex  _mutex;             /** Mutex for thread safety */
        QVariantMap     _datasetsMap;       /** Map of dataset IDs to dataset maps encountered during saving */
        QVariantMap     _dataHierarchyMap;  /** Map representing the data hierarchy structure */
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

UniqueWorkflowPlan DataHierarchyManager::fromVariantMapWorkflow(const QVariantMap& variantMap, SharedWorkflowExecutionContext parentExecutionContext)
{
    UniqueWorkflowPlan plan = std::make_unique<WorkflowPlan>(__FUNCTION__);

    const auto loadDataHierarchyItem = [parentExecutionContext](const QVariantMap& dataHierarchyItemMap, const QString& guiName, Dataset<DatasetImpl> parent) -> Dataset<DatasetImpl> {
        const auto dataset          = dataHierarchyItemMap["Dataset"].toMap();
        const auto datasetId        = dataset["ID"].toString();
        const auto datasetName      = dataset["Name"].toString();
        const auto pluginKind       = dataset["PluginKind"].toString();
        const auto isDerived        = dataset["Derived"].toBool();
        const auto isFull           = dataset["Full"].toBool();
        const auto sourceDatasetID  = dataset["SourceDatasetID"].toString();

        auto loadedDataset = (isDerived || !isFull) ? mv::data().createDatasetWithoutSelection(pluginKind, guiName, parent, datasetId) : mv::data().createDataset(pluginKind, guiName, parent, datasetId);

        if (isDerived)
            loadedDataset->setSourceDataset(sourceDatasetID);

        loadedDataset->getDataHierarchyItem().fromVariantMapScoped(dataHierarchyItemMap, parentExecutionContext);

        return loadedDataset;
    };

    const std::function<void(const QVariantMap&, Dataset<DatasetImpl>)> populateDataHierarchy = [&populateDataHierarchy, loadDataHierarchyItem](const QVariantMap& variantMap, Dataset<DatasetImpl> parent) -> void {
        QVector<QPair<QString, std::int32_t>> sortedDatasets;

        sortedDatasets.reserve(variantMap.keys().size());

        for (const auto& datasetId : variantMap.keys()) {
            const auto sortIndex = variantMap[datasetId].toMap()["SortIndex"].toInt();
            sortedDatasets.emplace_back(datasetId, sortIndex);
        }

        std::sort(sortedDatasets.begin(), sortedDatasets.end(), [](const auto& a, const auto& b) {
            return a.second < b.second;  // ascending
        });

        for (const auto& [datasetId, sortIndex] : sortedDatasets)
            populateDataHierarchy(variantMap[datasetId].toMap()["Children"].toMap(), loadDataHierarchyItem(variantMap[datasetId].toMap(), variantMap[datasetId].toMap()["Name"].toString(), parent));
    };

    populateDataHierarchy(variantMap[getSerializationName()].toMap(), Dataset<>());

    std::vector<QVariantMap> datasetMaps;

    const std::function<void(const QVariantMap&)> enumerateDatasetNames = [&enumerateDatasetNames, &datasetMaps](const QVariantMap& variantMap) -> void {
        for (const auto& variant : variantMap.values()) {
            enumerateDatasetNames(variant.toMap()["Children"].toMap());

            const auto datasetMap = variant.toMap()["Dataset"].toMap();

            datasetMaps.emplace_back(datasetMap);
        }
        };

    enumerateDatasetNames(variantMap[getSerializationName()].toMap());

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

    //for (const auto& dataVariantMap : datasetMaps) {
    //    const auto datasetId    = dataVariantMap["ID"].toString();
    //    const auto datasetName  = dataVariantMap["Name"].toString();

    //    plan->addNestedWorkflowStage(datasetName, [datasetId, dataVariantMap](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) -> UniqueWorkflowPlan {
    //        return mv::data().getDataset(datasetId)->fromVariantMapWorkflow(dataVariantMap, executionContext);
    //    });
    //}

    WorkflowPlan::Jobs datasetJobs;

    for (const auto& dataVariantMap : datasetMaps) {
        const auto datasetId = dataVariantMap["ID"].toString();
        const auto datasetName = dataVariantMap["Name"].toString();

        datasetJobs.emplace_back(
            datasetName,
            [datasetId, dataVariantMap](
                const WorkflowPlan::Job&,
                const SharedWorkflowExecutionContext& executionContext) {

                    auto dataset = mv::data().getDataset(datasetId);

                    auto nestedPlan = dataset->fromVariantMapWorkflow(
                        dataVariantMap,
                        executionContext);

                    WorkflowRuntimeScoped::instance().executeBlocking(
                        std::move(nestedPlan),
                        executionContext);
            });
    }

    plan->addParallelStage("Load datasets", std::move(datasetJobs));

    plan->addSequentialStage("Notify datasets", [this](const WorkflowPlan::Job& job) {
        for (const auto& item : _items) {
            events().notifyDatasetDataChanged(item->getDataset());
        }
	});

    return plan;
}

QVariantMap DataHierarchyManager::toVariantMapScoped(SharedWorkflowExecutionContext parentExecutionContext) const
{
    auto context = std::make_shared<ToVariantMapWorkflowContext>();

    UniqueWorkflowPlan plan = std::make_unique<WorkflowPlan>(__FUNCTION__, context);

    if (!_items.empty()) {
        WorkflowPlan::Jobs createItems;

        QStringList ids;

        std::int32_t sortIndex = 0;

        for (auto& dataHierarchyItem : _items) {
            const auto dataset = dataHierarchyItem->getDataset();
            const auto datasetId = dataset->getId();
            const auto datasetGuiName = dataset->getGuiName();
            const auto itemSortIndex = sortIndex++;

            ids << datasetId;
            createItems.emplace_back(datasetId, [context, &dataHierarchyItem, datasetId, itemSortIndex, datasetGuiName](const WorkflowPlan::Job& job, const SharedWorkflowExecutionContext& executionContext) {
                const auto itemMap = dataHierarchyItem->toVariantMapScoped(executionContext);

            	itemMap["SortIndex"] = itemSortIndex;

                context->setDataset(datasetId, itemMap);
            });
        }

        WorkflowPlan::Jobs loadDatasets;

        plan->addParallelStage("Create items", std::move(createItems));
        plan->addSequentialStage("Assemble hierarchy", [this, &plan, context](const WorkflowPlan::Job& job, const SharedWorkflowExecutionContext& executionContext) -> void {
            const auto findItemMap = [&plan, &job, context](const QString& datasetId) -> QVariantMap {
                return context->getDataset(datasetId);
            };

            std::function<QVariantMap(DataHierarchyItem*, QVariantMap&, std::int32_t)> traverseItem;

            traverseItem = [findItemMap, &traverseItem](DataHierarchyItem* item, QVariantMap& parentMap, std::int32_t sortIndex) -> QVariantMap {
                const auto datasetId = item->getDataset()->getId();

                std::uint32_t childSortIndex = 0;

                QVariantMap children;

                for (auto childItem : item->getChildren()) {
                    auto itemMap = findItemMap(childItem->getDataset()->getId());

                    itemMap["SortIndex"] = childSortIndex;

                    //children[datasetId] = traverseItem(childItem, children, childSortIndex);
                    children[childItem->getDataset()->getId()] = traverseItem(childItem, children, childSortIndex);

                    childSortIndex++;
                }

                auto datasetMap = findItemMap(datasetId);

                datasetMap["Children"] = children;
                datasetMap["SortIndex"] = sortIndex;

                parentMap[datasetId] = datasetMap;

                return datasetMap;
                };

            QVariantMap dataHierarchyMap;

            for (const auto topLevelItem : const_cast<DataHierarchyManager*>(this)->getTopLevelItems()) {
                dataHierarchyMap[topLevelItem->getDataset()->getId()] = traverseItem(topLevelItem, dataHierarchyMap, 0);
            }

            context->setDataHierarchyMap(dataHierarchyMap);

            executionContext->publishResultValue(getSerializationName(), dataHierarchyMap);
        });
    }

    auto result = Application::getWorkflowPlanExecutor().executeBlocking(std::move(plan), parentExecutionContext);

    return result->value<QVariantMap>()[getSerializationName()].toMap();
}

}