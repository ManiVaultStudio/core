#include "DataHierarchyModelItem.h"
#include "Set.h"
#include "Application.h"
#include "DataHierarchyManager.h"

#include <util/Icon.h>

#include <QDebug>
#include <QPainter>
#include <QMenu>

namespace hdps
{

using namespace gui;

QRandomGenerator DataHierarchyModelItem::rng = QRandomGenerator();

DataHierarchyModelItem::DataHierarchyModelItem(DataHierarchyItem* dataHierarchyItem, DataHierarchyModelItem* parent /*= nullptr*/) :
    QObject(parent),
    _parent(parent),
    _children(),
    _dataHierarchyItem(dataHierarchyItem)
{
}

DataHierarchyModelItem::~DataHierarchyModelItem()
{
    if (_parent)
        _parent->removeChild(this);

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

    return _dataHierarchyItem->getGuiName() + "\n" + _dataHierarchyItem->getDataset()->getGuid() + "\n" + _dataHierarchyItem->getDataType();
}

QVariant DataHierarchyModelItem::getDataAtColumn(const std::uint32_t& column, int role /*= Qt::DisplayRole*/) const
{
    if (_dataHierarchyItem == nullptr)
        return QVariant();

    switch (role)
    {
        // String representation
        case Qt::DisplayRole:
        {
            // Get edit value role for column
            const auto editValue = getDataAtColumn(column, Qt::EditRole);

            switch (static_cast<Column>(column))
            {
                case Column::Name:
                    return editValue;

                case Column::GUID:
                    return editValue;

                case Column::GroupIndex:
                    return editValue.toInt() >= 0 ? editValue : "";

                case Column::Description:
                    return editValue;

                case Column::Analysis:
                    return editValue;

                case Column::Progress:
                    return _dataHierarchyItem->isRunning() ? QString("%1%").arg(QString::number(editValue.toFloat(), 'f', 1)) : "";

                case Column::Analyzing:
                    return editValue;

                case Column::Locked:
                    return "";

                default:
                    break;
            }

            break;
        }

        // Actual value
        case Qt::EditRole:
        {
            switch (static_cast<Column>(column))
            {
                case Column::Name:
                    return _dataHierarchyItem->getGuiName();

                case Column::GUID:
                    return _dataHierarchyItem->getDataset()->getGuid();

                case Column::GroupIndex:
                    return _dataHierarchyItem->getDataset()->getGroupIndex();

                case Column::Description:
                    return _dataHierarchyItem->getTaskDescription();

                case Column::Analysis:
                    return "";

                case Column::Progress:
                    return _dataHierarchyItem->isRunning() ? 100.0f * _dataHierarchyItem->getTaskProgress() : 0.0f;

                case Column::Analyzing:
                    return "";

                case Column::Locked:
                    return _dataHierarchyItem->getLocked();

                default:
                    break;
            }

            break;
        }

        case Qt::DecorationRole:
        {
            auto& fontAwesome = hdps::Application::getIconFont("FontAwesome");

            switch (static_cast<Column>(column))
            {
                case Column::Name:
                    return _dataHierarchyItem->getIconByName("data");

                case Column::GUID:
                    break;

                case Column::GroupIndex:
                    break;

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

                case Column::Locked:
                {
                    if (_dataHierarchyItem->getDataset()->isLocked())
                        return fontAwesome.getIcon("lock");

                    break;
                }

                default:
                    break;
            }

            break;
        }

        case SupplementaryIcons::Icon1:
        {
            if (_dataHierarchyItem->getIcons().values().count() < 2)
                break;

            return _dataHierarchyItem->getIcons().values()[1];
        }

        case SupplementaryIcons::Icon2:
        {
            if (_dataHierarchyItem->getIcons().values().count() < 3)
                break;

            return _dataHierarchyItem->getIcons().values()[2];
        }

        case SupplementaryIcons::Icon3:
        {
            if (_dataHierarchyItem->getIcons().values().count() < 4)
                break;

            return _dataHierarchyItem->getIcons().values()[3];
        }

        // Grayed out text when locked
        case Qt::TextAlignmentRole:
        {
            switch (static_cast<Column>(column))
            {
                case Column::Name:
                case Column::GUID:
                    break;

                case Column::GroupIndex:
                    return static_cast<std::int32_t>(Qt::AlignVCenter | Qt::AlignRight);

                case Column::Analysis:
                case Column::Progress:
                case Column::Description:
                case Column::Analyzing:
                case Column::Locked:
                    break;

                default:
                    break;
            }

            break;
        }

        // Grayed out text when locked
        case Qt::ForegroundRole:
            return _dataHierarchyItem->getLocked() ? QColor(Qt::gray) : QColor(Qt::black);

        default:
            break;
    }

    return QVariant();
}

QMenu* DataHierarchyModelItem::getContextMenu()
{
    if (_dataHierarchyItem == nullptr || _dataHierarchyItem->getGuiName().isEmpty())
        return new QMenu();

    return _dataHierarchyItem->getContextMenu();
}

void DataHierarchyModelItem::renameDataset(const QString& intendedDatasetName)
{
    _dataHierarchyItem->renameDataset(intendedDatasetName);
}

void DataHierarchyModelItem::setGroupIndex(const std::int32_t& groupIndex)
{
    _dataHierarchyItem->getDataset()->setGroupIndex(groupIndex);
}

DataHierarchyItem* DataHierarchyModelItem::getDataHierarchyItem()
{
    return _dataHierarchyItem;
}

void DataHierarchyModelItem::removeChild(DataHierarchyModelItem* dataHierarchyModelItem)
{
    _children.removeOne(dataHierarchyModelItem);
}

}
