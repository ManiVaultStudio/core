#include "DataHierarchyManager.h"

#include <QMessageBox>

#include <stdexcept>

namespace hdps
{

DataHierarchyManager::DataHierarchyManager(QObject* parent /*= nullptr*/) :
    QObject(parent),
    _dataHierarchyItems()
{
}

void DataHierarchyManager::addDataset(const QString& datasetName, const QString& parentDatasetName /*= ""*/, const bool& visible /*= true*/)
{
    try {
        if (datasetName.isEmpty())
            throw std::runtime_error("Dataset name is empty");

        const auto newDataHierarchyItem = SharedDataHierarchyItem(new DataHierarchyItem(this, datasetName, parentDatasetName, visible));

        _dataHierarchyItems << newDataHierarchyItem;

        if (!parentDatasetName.isEmpty())
            getHierarchyItem(parentDatasetName)->addChild(datasetName);

        emit hierarchyItemAdded(newDataHierarchyItem);
    }
    catch (std::exception& e) {
        QMessageBox::critical(nullptr, "Unable to add dataset to data hierarchy", e.what());
    }
}

bool DataHierarchyManager::removeDataset(const QString& datasetName)
{
    try {
        if (datasetName.isEmpty())
            throw std::runtime_error("Dataset name is empty");

        const auto hierarchyItem = getHierarchyItem(datasetName);

        if (hierarchyItem.isNull())
            throw std::runtime_error(QString("%1 does not exist in the data hierarchy").arg(datasetName).toLatin1());

        for (auto child : getChildren(getHierarchyItem(datasetName)))
            _dataHierarchyItems.remove(_dataHierarchyItems.indexOf(child));

        emit hierarchyItemRemoved(datasetName);

    }
    catch (std::exception& e) {
        QMessageBox::critical(nullptr, "Unable to remove dataset from data hierarchy", e.what());
    }

    return true;
}

const SharedDataHierarchyItem DataHierarchyManager::getHierarchyItem(const QString& datasetName) const
{
    return const_cast<DataHierarchyManager*>(this)->getHierarchyItem(datasetName);
}

SharedDataHierarchyItem DataHierarchyManager::getHierarchyItem(const QString& datasetName)
{
    Q_ASSERT(!datasetName.isEmpty());

    for (auto dataHierarchyItem : _dataHierarchyItems)
        if (dataHierarchyItem->getDatasetName() == datasetName)
            return dataHierarchyItem;

    return nullptr;
}

void DataHierarchyManager::selectHierarchyItem(const QString& datasetName)
{
    try {
        if (datasetName.isEmpty())
            throw std::runtime_error("Dataset name is empty");

        const auto hierarchyItem = getHierarchyItem(datasetName);

        if (hierarchyItem.isNull())
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

SharedDataHierarchyItems DataHierarchyManager::getChildren(SharedDataHierarchyItem& dataHierarchyItem, const bool& recursive /*= true*/)
{
    Q_ASSERT(!dataHierarchyItem.isNull());

    auto children = dataHierarchyItem->getChildren();

    if (recursive)
        for (auto child : children)
            children << getChildren(child, recursive);

    return children;
}

}
