#include "DataHierarchyModelItem.h"

#include <Set.h>
#include <Application.h>
#include <DataHierarchyItem.h>

#include <QDebug>
#include <QPainter>
#include <QMenu>

using namespace hdps;
using namespace hdps::gui;

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
        case Qt::DisplayRole:
        {
            const auto editValue = getDataAtColumn(column, Qt::EditRole);

            switch (static_cast<Column>(column))
            {
                case Column::Name:
                    return editValue;

                case Column::GUID:
                    return editValue;

                case Column::Info:
                    return editValue;

                case Column::Progress:
                    return _dataHierarchyItem->isRunning() ? QString("%1%").arg(QString::number(editValue.toFloat(), 'f', 1)) : "";

                case Column::GroupIndex:
                    return editValue.toInt() >= 0 ? editValue : "";

                case Column::IsGroup:
                    return "";

                case Column::IsAnalyzing:
                    return "";

                case Column::IsLocked:
                    return "";

                default:
                    break;
            }

            break;
        }

        case Qt::EditRole:
        {
            switch (static_cast<Column>(column))
            {
                case Column::Name:
                    return _dataHierarchyItem->getGuiName();

                case Column::GUID:
                    return _dataHierarchyItem->getDataset()->getGuid();

                case Column::Info:
                    return _dataHierarchyItem->getTaskDescription();

                case Column::Progress:
                    return _dataHierarchyItem->isRunning() ? 100.0f * _dataHierarchyItem->getTaskProgress() : 0.0f;

                case Column::GroupIndex:
                    return _dataHierarchyItem->getDataset()->getGroupIndex();

                case Column::IsGroup:
                    return _dataHierarchyItem->getDataset()->isProxy();

                case Column::IsAnalyzing:
                    return "";

                case Column::IsLocked:
                    return _dataHierarchyItem->getLocked();

                default:
                    break;
            }

            break;
        }

        case Qt::ToolTipRole:
        {
            switch (static_cast<Column>(column))
            {
                case Column::Name:
                    return _dataHierarchyItem->getGuiName();

                case Column::GUID:
                    return _dataHierarchyItem->getDataset()->getGuid();

                case Column::Info:
                    return _dataHierarchyItem->getTaskDescription();

                case Column::Progress:
                    return _dataHierarchyItem->isRunning() ? 100.0f * _dataHierarchyItem->getTaskProgress() : 0.0f;

                case Column::GroupIndex:
                    return _dataHierarchyItem->getDataset()->getGroupIndex();

                case Column::IsGroup:
                {
                    if (!_dataHierarchyItem->getDataset()->isProxy())
                        return "";

                    QStringList proxyDatasetNames;

                    for (const auto& proxyMember : _dataHierarchyItem->getDataset()->getProxyMembers())
                        proxyDatasetNames << proxyMember->getGuiName();

                    return proxyDatasetNames.join("\n");
                }

                case Column::IsAnalyzing:
                    return "";

                case Column::IsLocked:
                    return QString("Dataset is %1").arg(_dataHierarchyItem->getLocked() ? "locked" : "unlocked");

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
                    return _dataHierarchyItem->getIcon();

                case Column::GUID:
                case Column::Info:
                case Column::Progress:
                case Column::GroupIndex:
                    break;
                
                case Column::IsGroup:
                {
                    if (_dataHierarchyItem->getDataset()->isProxy())
                        return fontAwesome.getIcon("object-group");

                    break;
                }

                case Column::IsAnalyzing:
                {
                    if (_dataHierarchyItem->isRunning())
                        return fontAwesome.getIcon("microchip");

                    break;
                }

                case Column::IsLocked:
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

        case Qt::TextAlignmentRole:
        {
            switch (static_cast<Column>(column))
            {
                case Column::Name:
                case Column::GUID:
                case Column::Info:
                case Column::Progress:
                    break;

                case Column::GroupIndex:
                    return static_cast<std::int32_t>(Qt::AlignVCenter | Qt::AlignRight);

                case Column::IsGroup:
                case Column::IsAnalyzing:
                case Column::IsLocked:
                    break;

                default:
                    break;
            }

            break;
        }

        case Qt::ForegroundRole:
            return _dataHierarchyItem->getLocked() ? QColor(Qt::gray) : QColor(Qt::black);

        default:
            break;
    }

    return QVariant();
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

bool DataHierarchyModelItem::isVisible() const
{
    if (_dataHierarchyItem == nullptr)
        return false;

    return _dataHierarchyItem->isVisible();
}

void DataHierarchyModelItem::removeChild(DataHierarchyModelItem* dataHierarchyModelItem)
{
    _children.removeOne(dataHierarchyModelItem);
}