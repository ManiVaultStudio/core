#include "DataHierarchyManager.h"
#include "Core.h"

namespace hdps
{

DataHierarchyManager::DataHierarchyManager(QObject* parent /*= nullptr*/) :
    QObject(parent),
    _dataHierarchyItemsMap()
{
}

void DataHierarchyManager::addDataset(const QString& datasetName, const QString& parentDatasetName /*= ""*/, const bool& visibleInGui /*= true*/)
{
    Q_ASSERT(!datasetName.isEmpty());

    _dataHierarchyItemsMap[datasetName] = new DataHierarchyItem(this, datasetName, parentDatasetName, visibleInGui);

    if (!parentDatasetName.isEmpty() && _dataHierarchyItemsMap.contains(parentDatasetName))
        _dataHierarchyItemsMap[parentDatasetName]->addChild(datasetName);

    emit hierarchyItemAdded(*_dataHierarchyItemsMap[datasetName]);
}

bool DataHierarchyManager::removeDataset(const QString& datasetName)
{
    Q_ASSERT(!datasetName.isEmpty());

    if (datasetName.isEmpty() || !_dataHierarchyItemsMap.contains(datasetName))
        return false;

    for (auto child : getChildren(datasetName))
        _dataHierarchyItemsMap.remove(child);

    emit hierarchyItemRemoved(*_dataHierarchyItemsMap[datasetName]);

    _dataHierarchyItemsMap.remove(datasetName);

    return true;
}

void DataHierarchyManager::setItemDescription(const QString& datasetName, const QString& description)
{
    Q_ASSERT(!datasetName.isEmpty());

    if (datasetName.isEmpty() || !_dataHierarchyItemsMap.contains(datasetName))
        return;
}

void DataHierarchyManager::setItemProgress(const QString& datasetName, const double& progress)
{
    Q_ASSERT(!datasetName.isEmpty());

    if (datasetName.isEmpty() || !_dataHierarchyItemsMap.contains(datasetName))
        return;
}

const hdps::DataHierarchyItem& DataHierarchyManager::getHierarchyItem(const QString& datasetName) const
{
    return const_cast<DataHierarchyManager*>(this)->getHierarchyItem(datasetName);
}

hdps::DataHierarchyItem& DataHierarchyManager::getHierarchyItem(const QString& datasetName)
{
    Q_ASSERT(!datasetName.isEmpty());

    return *_dataHierarchyItemsMap[datasetName];
}

QStringList DataHierarchyManager::getChildren(const QString& datasetName, const bool& recursive /*= true*/)
{
    Q_ASSERT(!datasetName.isEmpty());

    if (datasetName.isEmpty() || !_dataHierarchyItemsMap.contains(datasetName))
        return QStringList();

    auto& dataHierarchyItem = _dataHierarchyItemsMap[datasetName];

    auto children = dataHierarchyItem->getChildren();

    if (recursive)
        children << getChildren(datasetName, recursive);

    return children;
}

}
