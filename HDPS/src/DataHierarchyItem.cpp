#include "DataHierarchyItem.h"
#include "Core.h"
#include "Set.h"

namespace hdps
{

Core* DataHierarchyItem::core = nullptr;

DataHierarchyItem::DataHierarchyItem(QObject* parent /*= nullptr*/, const QString& datasetName /*= ""*/, const QString& parentDatasetName /*= ""*/, const bool& visibleInGui /*= true*/) :
    QObject(parent),
    _datasetName(datasetName),
    _parent(parentDatasetName),
    _children(),
    _visibleInGui(visibleInGui),
    _progress(0.0)
{
}

QString DataHierarchyItem::getDatasetName() const
{
    return _datasetName;
}

QString DataHierarchyItem::getParent() const
{
    return _parent;
}

QStringList DataHierarchyItem::getChildren() const
{
    return _children;
}

std::uint32_t DataHierarchyItem::getNumberOfChildren() const
{
    return _children.count();
}

QString DataHierarchyItem::getVisibleInGui() const
{
    return _visibleInGui;
}

QString DataHierarchyItem::getDescription() const
{
    return _description;
}

void DataHierarchyItem::setDescription(const QString& description)
{
    if (description == _description)
        return;

    _description = description;

    emit descriptionChanged(_description);
}

float DataHierarchyItem::getProgress() const
{
    return _progress;
}

void DataHierarchyItem::setProgress(const float& progress)
{
    if (progress == _progress)
        return;

    _progress = progress;

    emit progressChanged(_progress);
}

void DataHierarchyItem::addChild(const QString& name)
{
    _children << name;
}

void DataHierarchyItem::removeChild(const QString& name)
{
    _children.removeAll(name);
}

QString DataHierarchyItem::toString() const
{
    return QString("Name=%1, Parent=%2, Children=[%3], Visible in GUI=%4").arg(_datasetName, _parent, _children.join(", "), _visibleInGui ? "true" : "false");
}

hdps::DataSet& DataHierarchyItem::getDataset() const
{
    Q_ASSERT(!_datasetName.isEmpty());

    return core->requestData(_datasetName);
}

hdps::DataType DataHierarchyItem::getDataType() const
{
    Q_ASSERT(!_datasetName.isEmpty());

    return core->requestData(_datasetName).getDataType();
}

}
