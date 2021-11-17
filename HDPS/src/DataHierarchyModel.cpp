#include "DataHierarchyModel.h"
#include "DataHierarchyModelItem.h"
#include "DataHierarchyItem.h"

#include <QDebug>
#include <QIcon>

namespace hdps
{

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

    switch (role) {
        case Qt::DecorationRole:
            return item->getIconAtColumn(index.column());

        case Qt::DisplayRole:
        case Qt::ToolTipRole:
        case Qt::EditRole:
            return item->getDataAtColumn(index.column());
    }

    return QVariant();
}

bool DataHierarchyModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/)
{
    const auto column = static_cast<DataHierarchyModelItem::Column>(index.column());

    auto dataHierarchyItem = static_cast<DataHierarchyModelItem*>((void*)index.internalPointer());
    
    switch (column) {
        case DataHierarchyModelItem::Column::Name:
            dataHierarchyItem->renameDataset(value.toString());
            break;

        case DataHierarchyModelItem::Column::ID:
            break;

        case DataHierarchyModelItem::Column::Description:
            dataHierarchyItem->setProgressSection(value.toString());
            break;

        case DataHierarchyModelItem::Column::Analysis:
            break;

        case DataHierarchyModelItem::Column::Progress:
            dataHierarchyItem->setProgressPercentage(value.toFloat());
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

    if (static_cast<DataHierarchyModelItem::Column>(index.column()) == DataHierarchyModelItem::Column::Name)
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

                    case DataHierarchyModelItem::Column::ID:
                        return "ID";

                    case DataHierarchyModelItem::Column::Description:
                        return "Description";

                    case DataHierarchyModelItem::Column::Analysis:
                    case DataHierarchyModelItem::Column::Analyzing:
                    case DataHierarchyModelItem::Column::Progress:
                        return "";

                    default:
                        break;
                }

                break;
            }

            case Qt::DecorationRole:
            {
                const auto iconSize = QSize(14, 14);

                switch (static_cast<DataHierarchyModelItem::Column>(section))
                {
                    case DataHierarchyModelItem::Column::Name:
                    case DataHierarchyModelItem::Column::ID:
                    case DataHierarchyModelItem::Column::Description:
                    case DataHierarchyModelItem::Column::Analysis:
                        break;

                    case DataHierarchyModelItem::Column::Analyzing:
                        return Application::getIconFont("FontAwesome").getIcon("check", iconSize);

                    case DataHierarchyModelItem::Column::Progress:
                        return Application::getIconFont("FontAwesome").getIcon("percentage", iconSize);

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

QMimeData* DataHierarchyModel::mimeData(const QModelIndexList &indexes) const
{
    QVector<DataHierarchyModelItem*> items;

    foreach(const QModelIndex &index, indexes)
        items.push_back(getItem(index, Qt::DisplayRole));

    QMimeData* mimeData = QAbstractItemModel::mimeData(indexes);

    mimeData->setText(items[0]->serialize());

    return mimeData;
}

bool DataHierarchyModel::addDataHierarchyModelItem(const QModelIndex& parentModelIndex, DataHierarchyItem* dataHierarchyItem)
{
    auto parentItem = !parentModelIndex.isValid() ? _rootItem : getItem(parentModelIndex, Qt::DisplayRole);

    emit layoutAboutToBeChanged();

    beginInsertRows(parentModelIndex, rowCount(parentModelIndex), rowCount(parentModelIndex) + 1);
    {
        parentItem->addChild(new DataHierarchyModelItem(dataHierarchyItem));
    }
    endInsertRows();

    emit layoutChanged();

    for (auto child : dataHierarchyItem->getChildren()) {
        addDataHierarchyModelItem(index(rowCount(parentModelIndex) - 1, 0, parentModelIndex), child);
    }

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

}
