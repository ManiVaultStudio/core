#include "DataHierarchyManager.h"
#include "DataManager.h"

#include <QMessageBox>

#include <stdexcept>

namespace hdps
{

DataHierarchyManager::DataHierarchyManager(DataManager& dataManager, QObject* parent /*= nullptr*/) :
    QObject(parent),
    _dataManager(dataManager),
    _dataHierarchyItems()
{
}

void DataHierarchyManager::addDataset(const QString& datasetName, const QString& parentDatasetName /*= ""*/, const bool& visible /*= true*/)
{
    try {
        if (datasetName.isEmpty())
            throw std::runtime_error("Dataset name is empty");

        const auto newDataHierarchyItem = SharedDataHierarchyItem::create(this, datasetName, parentDatasetName, visible);

        _dataHierarchyItems << newDataHierarchyItem;

        if (!parentDatasetName.isEmpty())
            getHierarchyItem(parentDatasetName)->addChild(datasetName);

        emit itemAdded(newDataHierarchyItem.get());
    }
    catch (std::exception& e) {
        QMessageBox::critical(nullptr, "Unable to add dataset to data hierarchy", e.what());
    }
}

bool DataHierarchyManager::removeDataset(const QString& datasetName, const bool& recursively /*= false*/)
{
    try {
        if (datasetName.isEmpty())
            throw std::runtime_error("Dataset name is empty");

        const auto hierarchyItem = getHierarchyItem(datasetName);

        if (hierarchyItem == nullptr)
            throw std::runtime_error(QString("%1 does not exist in the data hierarchy").arg(datasetName).toLatin1());

        removeDataHierarchyItemByDatasetName(hierarchyItem->getDatasetName(), recursively);

        // Remove the dataset from the data manager
        _dataManager.removeDataset(datasetName);
    }
    catch (std::exception& e) {
        QMessageBox::critical(nullptr, "Unable to remove dataset from data hierarchy", e.what());
    }

    return true;
}

const DataHierarchyItem* DataHierarchyManager::getHierarchyItem(const QString& datasetName) const
{
    return const_cast<DataHierarchyManager*>(this)->getHierarchyItem(datasetName);
}

DataHierarchyItem* DataHierarchyManager::getHierarchyItem(const QString& datasetName)
{
    Q_ASSERT(!datasetName.isEmpty());

    for (auto dataHierarchyItem : _dataHierarchyItems)
        if (dataHierarchyItem->getDatasetName() == datasetName)
            return dataHierarchyItem.get();

    return nullptr;
}

void DataHierarchyManager::selectHierarchyItem(const QString& datasetName)
{
    try {
        if (datasetName.isEmpty())
            throw std::runtime_error("Dataset name is empty");

        const auto hierarchyItem = getHierarchyItem(datasetName);

        if (hierarchyItem == nullptr)
            throw std::runtime_error(QString("%1 does not exist in the data hierarchy").arg(datasetName).toLatin1());

        for (auto dataHierarchyItem : _dataHierarchyItems)
            if (dataHierarchyItem->isSelected())
                dataHierarchyItem->deselect();

        hierarchyItem->select();
    }
    catch (std::exception& e) {
        QMessageBox::critical(nullptr, "Unable to select dataset in data hierarchy", e.what());
    }
}

DataHierarchyItems DataHierarchyManager::getChildren(DataHierarchyItem* dataHierarchyItem, const bool& recursive /*= true*/)
{
    Q_ASSERT(dataHierarchyItem != nullptr);

    auto children = dataHierarchyItem->getChildren();

    if (recursive)
        for (auto child : children)
            children << getChildren(child, recursive);

    return children;
}

void DataHierarchyManager::removeDataHierarchyItemByDatasetName(const QString& datasetName, const bool& recursively /*= false*/)
{
    for (auto dataHierarchyItem : _dataHierarchyItems) {
        if (dataHierarchyItem->getDatasetName() == datasetName) {
            if (recursively) {
                for (auto removeDataHierarchyItem : getChildren(dataHierarchyItem.get(), true))
                    removeDataHierarchyItemByDatasetName(removeDataHierarchyItem->getDatasetName(), recursively);
            }
            else {
                for (auto child : dataHierarchyItem->getChildren()) {
                    emit itemAboutToBeRelocated(child);
                    {
                        child->setParent(nullptr);
                    }
                    emit itemRelocated(child);
                }
            }

            emit itemAboutToBeRemoved(datasetName);
            {
                _dataHierarchyItems.removeOne(dataHierarchyItem);
            }
            emit itemRemoved(datasetName);
        }
    }
}

}
