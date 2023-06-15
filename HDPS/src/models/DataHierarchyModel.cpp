#include "DataHierarchyModel.h"
#include "DataHierarchyModelItem.h"
#include "DatasetsMimeData.h"

#include <DataHierarchyItem.h>

#include <QDebug>
#include <QIcon>

using namespace hdps;

DataHierarchyModel::DataHierarchyModel(QObject* parent) :
    QAbstractItemModel(parent),
    _rootItem(new DataHierarchyModelItem(nullptr))
{
}

DataHierarchyModel::~DataHierarchyModel()
{
    delete _rootItem;
}

QVariant DataHierarchyModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    auto item = static_cast<DataHierarchyModelItem*>(index.internalPointer());

    return item->getDataAtColumn(index.column(), role);
}

bool DataHierarchyModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/)
{
    const auto column = static_cast<DataHierarchyModelItem::Column>(index.column());

    auto dataHierarchyModelItem = static_cast<DataHierarchyModelItem*>((void*)index.internalPointer());
    
    switch (column) {
        case DataHierarchyModelItem::Column::Name:
            dataHierarchyModelItem->renameDataset(value.toString());
            break;

        case DataHierarchyModelItem::Column::GUID:
            break;

        case DataHierarchyModelItem::Column::Info:
            dataHierarchyModelItem->getDataHierarchyItem()->setTaskDescription(value.toString());
            break;

        case DataHierarchyModelItem::Column::Progress:
            dataHierarchyModelItem->getDataHierarchyItem()->setTaskProgress(value.toFloat());
            break;

        case DataHierarchyModelItem::Column::GroupIndex:
            dataHierarchyModelItem->setGroupIndex(value.toInt());
            break;

        case DataHierarchyModelItem::Column::IsGroup:
        case DataHierarchyModelItem::Column::IsAnalyzing:
        case DataHierarchyModelItem::Column::IsLocked:
            break;

        default:
            break;
    }

    emit dataChanged(index, index);

    return true;
}

QModelIndex DataHierarchyModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    DataHierarchyModelItem* parentItem;

    if (!parent.isValid())
        parentItem = _rootItem;
    else
        parentItem = static_cast<DataHierarchyModelItem*>(parent.internalPointer());

    auto childItem = parentItem->getChild(row);

    if (childItem)
        return createIndex(row, column, childItem);

    return QModelIndex();
}

QModelIndex DataHierarchyModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
        return QModelIndex();

    auto childItem  = static_cast<DataHierarchyModelItem*>(index.internalPointer());
    auto parentItem = childItem->getParent();

    if (parentItem == nullptr || parentItem == _rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int DataHierarchyModel::rowCount(const QModelIndex& parent) const
{
    DataHierarchyModelItem* parentItem;

    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = _rootItem;
    else
        parentItem = static_cast<DataHierarchyModelItem*>(parent.internalPointer());

    return parentItem->getNumChildren();
}

int DataHierarchyModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return static_cast<DataHierarchyModelItem*>(parent.internalPointer())->getNumColumns();

    return _rootItem->getNumColumns();
}

Qt::DropActions DataHierarchyModel::supportedDragActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

DataHierarchyModelItem* DataHierarchyModel::getItem(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return nullptr;

    if (role != Qt::DisplayRole)
        return nullptr;

    return static_cast<DataHierarchyModelItem*>(index.internalPointer());
}

Qt::ItemFlags DataHierarchyModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    auto itemFlags = Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | QAbstractItemModel::flags(index);

    auto dataHierarchyModelItem = static_cast<DataHierarchyModelItem*>(index.internalPointer());

    const auto itemIsLocked = dataHierarchyModelItem->getDataAtColumn(DataHierarchyModelItem::Column::IsLocked, Qt::EditRole).toBool();

    if (!itemIsLocked && static_cast<DataHierarchyModelItem::Column>(index.column()) == DataHierarchyModelItem::Column::Name)
        itemFlags |= Qt::ItemIsEditable;

    if (!itemIsLocked && static_cast<DataHierarchyModelItem::Column>(index.column()) == DataHierarchyModelItem::Column::GroupIndex)
        itemFlags |= Qt::ItemIsEditable;

    return itemFlags;
}

QVariant DataHierarchyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        switch (role) {
            case Qt::DisplayRole:
            {
                switch (static_cast<DataHierarchyModelItem::Column>(section))
                {
                    case DataHierarchyModelItem::Column::Name:
                        return "Name";

                    case DataHierarchyModelItem::Column::GUID:
                        return "ID";

                    case DataHierarchyModelItem::Column::Info:
                        return "Info";

                    case DataHierarchyModelItem::Column::Progress:
                    case DataHierarchyModelItem::Column::GroupIndex:
                    case DataHierarchyModelItem::Column::IsGroup:
                    case DataHierarchyModelItem::Column::IsAnalyzing:
                    case DataHierarchyModelItem::Column::IsLocked:
                        return "";

                    default:
                        break;
                }

                break;
            }

            case Qt::EditRole:
            {
                switch (static_cast<DataHierarchyModelItem::Column>(section))
                {
                    case DataHierarchyModelItem::Column::Name:
                        return "Name";

                    case DataHierarchyModelItem::Column::GUID:
                        return "ID";

                    case DataHierarchyModelItem::Column::Info:
                        return "Info";

                    case DataHierarchyModelItem::Column::Progress:
                        return "Progress";

                    case DataHierarchyModelItem::Column::GroupIndex:
                        return "Group ID";

                    case DataHierarchyModelItem::Column::IsGroup:
                        return "Is group";

                    case DataHierarchyModelItem::Column::IsAnalyzing:
                        return "Is analyzing";

                    case DataHierarchyModelItem::Column::IsLocked:
                        return "Is locked";

                    default:
                        break;
                }

                break;
            }

            case Qt::ToolTipRole:
            {
                const auto iconSize = QSize(14, 14);

                switch (static_cast<DataHierarchyModelItem::Column>(section))
                {
                    case DataHierarchyModelItem::Column::Name:
                        return "Name of the dataset";

                    case DataHierarchyModelItem::Column::GUID:
                        return "Globally unique dataset identifier";

                    case DataHierarchyModelItem::Column::Info:
                        return "Dataset additional information";

                    case DataHierarchyModelItem::Column::Progress:
                        return "Task progress in percentage";

                    case DataHierarchyModelItem::Column::GroupIndex:
                        return "Dataset group index";

                    case DataHierarchyModelItem::Column::IsGroup:
                        return "Whether the dataset is composed of other datasets";

                    case DataHierarchyModelItem::Column::IsAnalyzing:
                        return "Whether an analysis is taking place on the dataset";

                    case DataHierarchyModelItem::Column::IsLocked:
                        return "Whether the dataset is locked";

                    default:
                        break;
                }

                break;
            }

            default:
                break;
        }
    }

    return QVariant();
}

QMimeData* DataHierarchyModel::mimeData(const QModelIndexList& indexes) const
{
    Datasets datasets;

    QVector<DataHierarchyModelItem*> items;

    foreach(const QModelIndex &index, indexes)
        datasets << getItem(index, Qt::DisplayRole)->getDataHierarchyItem()->getDataset();

    return new DatasetsMimeData(datasets);
}

bool DataHierarchyModel::addDataHierarchyModelItem(const QModelIndex& parentModelIndex, hdps::DataHierarchyItem& dataHierarchyItem)
{
    auto parentItem = !parentModelIndex.isValid() ? _rootItem : getItem(parentModelIndex, Qt::DisplayRole);

    emit layoutAboutToBeChanged();
    {
        beginInsertRows(parentModelIndex, rowCount(parentModelIndex), rowCount(parentModelIndex) + 1);
        {
            parentItem->addChild(new DataHierarchyModelItem(&dataHierarchyItem));
        }
        endInsertRows();
    }
    emit layoutChanged();

    for (auto child : dataHierarchyItem.getChildren())
        addDataHierarchyModelItem(index(rowCount(parentModelIndex) - 1, 0, parentModelIndex), *child);

    return true;
}

bool DataHierarchyModel::removeDataHierarchyModelItem(const QModelIndex& modelIndex)
{
    auto dataHierarchyModelItem = static_cast<DataHierarchyModelItem*>(modelIndex.internalPointer());

    beginRemoveRows(modelIndex.parent(), modelIndex.row(), modelIndex.row());
    {
        delete dataHierarchyModelItem;
    }
    endRemoveRows();

    return true;
}
