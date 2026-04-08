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

QFuture<void> fromVariantMapAsync(WidgetAction* action, const QVariantMap& variantMap)
{
    Q_ASSERT(action);

    if (!action)
        return {};

    return QtConcurrent::run([action, &variantMap]() -> void {
        action->fromVariantMap(variantMap);
    });
}

void DataHierarchyManager::fromVariantMap(const QVariantMap& variantMap)
{
    //if (Application::isSerializationAborted())
    //    return;

    //// Cancel any existing load
    //if (_activeLoadWorkflow)
    //    _activeLoadWorkflow->cancel();

    //_activeLoadWorkflow = std::make_unique<DataHierarchyLoadWorkflow>(*this, this);

    //connect(_activeLoadWorkflow.get(), &DataHierarchyLoadWorkflow::finished,
    //    this,
    //    [this](bool success, const QString& error) {
    //        if (!success) {
    //            qWarning() << "Failed to load data hierarchy:" << error;
    //        }

    //        _activeLoadWorkflow.reset();
    //    });

    //_activeLoadWorkflow->setInput(variantMap);
    //_activeLoadWorkflow->start();
}

QVariantMap DataHierarchyManager::toVariantMap() const
{
    if (!_items.empty()) {
        auto toPlan = makeToPlan();

        toPlan.addSequentialStage("Enumerate datasets", [this](SerializationPlan::Job& job) -> void {
            try {
	            
            } catch (std::exception& e) {
                Serializable::reportSerializationError("Data hierarchy manager", "Failed to Enumerate datasets: " + QString::fromStdString(e.what()));
            }
            catch (...) {
                Serializable::reportSerializationError("Data hierarchy manager", "Failed to Enumerate datasets");
            }
        });

        SerializationPlan::Jobs createItemMapJobs;

        std::int32_t sortIndex = 0;

        for (auto& dataHierarchyItem : _items) {
            const auto dataset          = dataHierarchyItem->getDataset();
            const auto datasetId        = dataset->getId();
            const auto datasetGuiName   = dataset->getGuiName();

            createItemMapJobs.emplace_back(datasetGuiName, [&toPlan, datasetId, &datasetGuiName, &sortIndex, &dataHierarchyItem](SerializationPlan::Job& job) {
                try {
                    const auto itemMap = dataHierarchyItem->toVariantMap();

                    itemMap["SortIndex"] = sortIndex;

                    (*toPlan.getSharedState())[datasetId] = itemMap;
                } catch (std::exception& e) {
                    Serializable::reportSerializationError("Data hierarchy manager", "Failed to save dataset: " + datasetGuiName + ": " + QString::fromStdString(e.what()));
                }
                catch (...) {
                    Serializable::reportSerializationError("Data hierarchy manager", "Failed to save dataset: " + datasetGuiName);
                }
            });

            sortIndex++;
        }

        toPlan.addParallelStage("Create item maps", createItemMapJobs);

        toPlan.addSequentialStage("Assemble item maps", [this, &toPlan](SerializationPlan::Job& job) -> void {
            try {
                QVariantMap variantMap;

                const auto findItemMap = [&toPlan](const QString& datasetId) -> QVariantMap {
                    const auto datasetIds = toPlan.getSharedState()->keys();

                    const auto it = std::find_if(datasetIds.begin(), datasetIds.end(), [&datasetId](const auto& candidateDatasetId) -> bool {
                        return candidateDatasetId == datasetId;
                    });

                    if (it == datasetIds.end()) {
                        throw std::runtime_error(QString("Failed to find serialization job for dataset ID %1").arg(datasetId).toStdString());
                    }

                	return (*toPlan.getSharedState())[datasetId].toMap();
                };

                std::function<QVariantMap(DataHierarchyItem*, QVariantMap&)> traverseItem;

                traverseItem = [findItemMap, &traverseItem](DataHierarchyItem* item, QVariantMap& parentMap) -> QVariantMap {
                    const auto datasetId = item->getDataset()->getId();

                    parentMap[datasetId] = findItemMap(datasetId);

                	std::uint32_t childSortIndex = 0;

                    QVariantMap children;

                    for (auto childItem : item->getChildren()) {
			            auto itemMap = findItemMap(childItem->getDataset()->getId());

			            itemMap["SortIndex"] = childSortIndex;

			            children[datasetId] = traverseItem(childItem, children);
			            childSortIndex++;
			        }

                    parentMap["Children"] = children;

                    return parentMap;
                };

                for (const auto topLevelItem : const_cast<DataHierarchyManager*>(this)->getTopLevelItems()) {
                    variantMap[topLevelItem->getDataset()->getId()] = traverseItem(topLevelItem, variantMap);
                }

                job.setResult(variantMap);
            } catch (std::exception& e) {
                Serializable::reportSerializationError("Data hierarchy manager", "Failed to assemble item maps: " + QString::fromStdString(e.what()));
            }
            catch (...) {
                Serializable::reportSerializationError("Data hierarchy manager", "Failed to assemble item maps");
            }
        });

        toPlan.execute(*mv::projects().getSerializationPlanExecutor());

        return toPlan.getResult().toMap();
    }
    
    return {};
}

}
