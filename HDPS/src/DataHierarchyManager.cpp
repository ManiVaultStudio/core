#include "DataHierarchyManager.h"
#include "Core.h"

namespace hdps
{

QString DataHierarchyManager::DataHierarchyItem::getDatasetName() const
{
    return _datasetName;
}

QString DataHierarchyManager::DataHierarchyItem::getParent() const
{
    return _parent;
}

QStringList DataHierarchyManager::DataHierarchyItem::getChildren() const
{
    return _children;
}

std::uint32_t DataHierarchyManager::DataHierarchyItem::getNumberOfChildren() const
{
    return _children.count();
}

QString DataHierarchyManager::DataHierarchyItem::getVisibleInGui() const
{
    return _visibleInGui;
}

QString DataHierarchyManager::DataHierarchyItem::getDescription() const
{
    return _description;
}

void DataHierarchyManager::DataHierarchyItem::addChild(const QString& name)
{
    _children << name;
}

void DataHierarchyManager::DataHierarchyItem::removeChild(const QString& name)
{
    _children.removeAll(name);
}

QString DataHierarchyManager::DataHierarchyItem::toString() const
{
    return QString("Name=%1, Parent=%2, Children=[%3], Visible in GUI=%4").arg(_datasetName, _parent, _children.join(", "), _visibleInGui ? "true" : "false");
}

hdps::DataSet& DataHierarchyManager::DataHierarchyItem::getDataset() const
{
    Q_ASSERT(!_datasetName.isEmpty());

    return core->requestData(_datasetName);
}

hdps::DataType DataHierarchyManager::DataHierarchyItem::getDataType() const
{
    Q_ASSERT(!_datasetName.isEmpty());

    return core->requestData(_datasetName).getDataType();
}

Core* DataHierarchyManager::DataHierarchyItem::core = nullptr;

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

    emit hierarchyItemAdded(_dataHierarchyItemsMap[datasetName]);
}

bool DataHierarchyManager::removeDataset(const QString& datasetName)
{
    Q_ASSERT(!datasetName.isEmpty());

    if (datasetName.isEmpty() || !_dataHierarchyItemsMap.contains(datasetName))
        return false;

    for (auto child : getChildren(datasetName))
        _dataHierarchyItemsMap.remove(child);

    emit hierarchyItemRemoved(_dataHierarchyItemsMap[datasetName]);

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
