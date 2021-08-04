#include "DataHierarchyManager.h"

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

    _dataHierarchyItemsMap[datasetName] = DataHierarchyItem(datasetName, parentDatasetName, visibleInGui);

    if (!parentDatasetName.isEmpty() && _dataHierarchyItemsMap.contains(parentDatasetName))
        _dataHierarchyItemsMap[parentDatasetName].addChild(datasetName);

    emit itemAdded(_dataHierarchyItemsMap[datasetName]);
}

bool DataHierarchyManager::removeDataset(const QString& datasetName)
{
    Q_ASSERT(!datasetName.isEmpty());

    if (datasetName.isEmpty() || !_dataHierarchyItemsMap.contains(datasetName))
        return false;

    for (auto child : getChildren(datasetName))
        _dataHierarchyItemsMap.remove(child);

    emit itemRemoved(_dataHierarchyItemsMap[datasetName]);

    _dataHierarchyItemsMap.remove(datasetName);

    return true;
}

const hdps::DataHierarchyManager::DataHierarchyItem& DataHierarchyManager::getHierarchyItem(const QString& datasetName) const
{
    Q_ASSERT(!datasetName.isEmpty());

    if (datasetName.isEmpty() || !_dataHierarchyItemsMap.contains(datasetName))
        return DataHierarchyItem();

    return _dataHierarchyItemsMap[datasetName];
}

QStringList DataHierarchyManager::getChildren(const QString& datasetName, const bool& recursive /*= true*/)
{
    Q_ASSERT(!datasetName.isEmpty());

    if (datasetName.isEmpty() || !_dataHierarchyItemsMap.contains(datasetName))
        return QStringList();

    auto& dataHierarchyItem = _dataHierarchyItemsMap[datasetName];

    auto children = dataHierarchyItem.getChildren();

    if (recursive)
        children << getChildren(datasetName, recursive);

    return children;
}

}
