#include "DataHierarchyItem.h"
#include "Core.h"
#include "Set.h"

namespace hdps
{

Core* DataHierarchyItem::core = nullptr;

DataHierarchyItem::DataHierarchyItem(QObject* parent /*= nullptr*/, const QString& datasetName /*= ""*/, const QString& parentDatasetName /*= ""*/, const bool& visible /*= true*/, const bool& selected /*= false*/) :
    QObject(parent),
    _datasetName(datasetName),
    _parent(parentDatasetName),
    _children(),
    _visible(visible),
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

bool DataHierarchyItem::getVisible() const
{
    return _visible;
}

bool DataHierarchyItem::isHidden() const
{
    return !_visible;
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

bool DataHierarchyItem::isSelected() const
{
    return _selected;
}

void DataHierarchyItem::setSelected(const bool& selected)
{
    if (selected == _selected)
        return;

    _selected = selected;

    emit selectionChanged(_selected);
}

void DataHierarchyItem::select()
{
    setSelected(true);
}

void DataHierarchyItem::deselect()
{
    setSelected(false);
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
    return QString("DataHierarchyItem[name=%1, parent=%2, children=[%3], visible=%4, description=%5, progress=%6]").arg(_datasetName, _parent, _children.join(", "), _visible ? "true" : "false", _description, QString::number(_progress, 'f', 1));
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
