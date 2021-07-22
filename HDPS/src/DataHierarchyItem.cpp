#include "DataHierarchyItem.h"
#include "Set.h"
#include "DataExportAction.h"
#include "DataAnalysisAction.h"

#include <QDebug>

namespace hdps
{

Core* DataHierarchyItem::core = nullptr;

DataHierarchyItem::DataHierarchyItem(const QString& datasetName /*= ""*/, DataHierarchyItem* parent /*= nullptr*/) :
    QObject(parent),
    _parent(parent),
    _children(),
    _datasetName(datasetName),
    _dataset(nullptr)
{
    if (!datasetName.isEmpty()) {
        try
        {
            _dataset = dynamic_cast<DataSet*>(&core->requestData(_datasetName));
        }
        catch (const std::exception&)
        {
        }
    }
}

DataHierarchyItem::~DataHierarchyItem()
{
    qDeleteAll(_children);
}

void DataHierarchyItem::addChild(DataHierarchyItem* item)
{
    item->setParent(this);
    _children.append(item);
}

DataHierarchyItem* DataHierarchyItem::getParent()
{
    return _parent;
}

void DataHierarchyItem::setParent(DataHierarchyItem* parent)
{
    _parent = parent;
}

DataHierarchyItem* DataHierarchyItem::getChild(const std::int32_t& row)
{
    if (row < 0 || row >= _children.size())
        return nullptr;

    return _children[row];
}

std::int32_t DataHierarchyItem::row() const
{
    if (_parent)
        return _parent->_children.indexOf(const_cast<DataHierarchyItem*>(this));

    return 0;
}

std::int32_t DataHierarchyItem::getNumChildren() const
{
    return _children.count();
}

std::int32_t DataHierarchyItem::getNumColumns() const
{
    return 2;
}

QString DataHierarchyItem::serialize() const
{
    return "";
    //return _setName + "\n" + _dataType;
}

QString DataHierarchyItem::getDataAtColumn(const std::uint32_t& column) const
{
    if (_datasetName.isEmpty())
        return "Data";

    Q_ASSERT(_dataset != nullptr);

    return _dataset->getName();
}

QIcon DataHierarchyItem::getIcon()
{
    return hdps::Application::getIconFont("FontAwesome").getIcon(_datasetName.isEmpty() ? "home" : "database");
}

QMenu* DataHierarchyItem::getContextMenu()
{
    if (_datasetName.isEmpty())
        return new QMenu();

    Q_ASSERT(_dataset != nullptr);

    return _dataset->getContextMenu();
}

QString DataHierarchyItem::getDatasetName() const
{
    return _datasetName;
}

}