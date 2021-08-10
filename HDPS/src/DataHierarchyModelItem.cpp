#include "DataHierarchyModelItem.h"
#include "Set.h"
#include "ExportDataAction.h"
#include "AnalyzeDataAction.h"

#include <QDebug>

namespace hdps
{

DataHierarchyModelItem::DataHierarchyModelItem(DataHierarchyItem* dataHierarchyItem, DataHierarchyModelItem* parent /*= nullptr*/) :
    QObject(parent),
    _parent(parent),
    _children(),
    _dataHierarchyItem(dataHierarchyItem),
    _progressSection(),
    _progressPercentage(0.0f)
{
}

DataHierarchyModelItem::~DataHierarchyModelItem()
{
    qDeleteAll(_children);
}

void DataHierarchyModelItem::addChild(DataHierarchyModelItem* item)
{
    item->setParent(this);
    _children.append(item);
}

DataHierarchyModelItem* DataHierarchyModelItem::getParent()
{
    return _parent;
}

void DataHierarchyModelItem::setParent(DataHierarchyModelItem* parent)
{
    _parent = parent;
}

DataHierarchyModelItem* DataHierarchyModelItem::getChild(const std::int32_t& row)
{
    if (row < 0 || row >= _children.size())
        return nullptr;

    return _children[row];
}

std::int32_t DataHierarchyModelItem::row() const
{
    if (_parent)
        return _parent->_children.indexOf(const_cast<DataHierarchyModelItem*>(this));

    return 0;
}

std::int32_t DataHierarchyModelItem::getNumChildren() const
{
    return _children.count();
}

std::int32_t DataHierarchyModelItem::getNumColumns() const
{
    return static_cast<std::int32_t>(Column::_end) + 1;
}

QString DataHierarchyModelItem::serialize() const
{
    if (_dataHierarchyItem == nullptr)
        return "";

    return _dataHierarchyItem->getDatasetName() + "\n" + _dataHierarchyItem->getDataType();
}

QString DataHierarchyModelItem::getDataAtColumn(const std::uint32_t& column) const
{
    if (_dataHierarchyItem == nullptr)
        return "";

    if (_dataHierarchyItem->getDatasetName().isEmpty())
        return "Data";

    switch (static_cast<Column>(column))
    {
        case Column::Name:
            return _dataHierarchyItem->getDatasetName();

        case Column::Description:
            return _progressSection;

        case Column::Analysis:
            return "";

        case Column::Progress:
            return _dataHierarchyItem->isRunning() ? QString("%1%").arg(QString::number(100.0f * _progressPercentage, 'f', 1)) : "";

        case Column::Analyzing:
            return "";

        default:
            break;
    }

    return "";
}

QIcon DataHierarchyModelItem::getIconAtColumn(const std::uint32_t& column) const
{
    auto& fontAwesome = hdps::Application::getIconFont("FontAwesome");

    if (_dataHierarchyItem == nullptr)
        return fontAwesome.getIcon("home");

    switch (static_cast<Column>(column))
    {
        case Column::Name:
            return _dataHierarchyItem->getIconByName("data");

        case Column::Analysis:
            return _dataHierarchyItem->getIconByName("analysis");

        case Column::Progress:
        case Column::Description:
            break;

        case Column::Analyzing:
        {
            if (_dataHierarchyItem->isRunning())
                return fontAwesome.getIcon("microchip");

            break;
        }

        default:
            break;
    }

    return QIcon();
}

QMenu* DataHierarchyModelItem::getContextMenu()
{
    if (_dataHierarchyItem == nullptr || _dataHierarchyItem->getDatasetName().isEmpty())
        return new QMenu();

    return _dataHierarchyItem->getDataset().getContextMenu();
}

void DataHierarchyModelItem::renameDataset(const QString& intendedDatasetName)
{
    _dataHierarchyItem->renameDataset(intendedDatasetName);
}

void DataHierarchyModelItem::setProgressPercentage(const float& progressPercentage)
{
    _progressPercentage = progressPercentage;
}

void DataHierarchyModelItem::setProgressSection(const QString& progressSection)
{
    _progressSection = progressSection;
}

}
