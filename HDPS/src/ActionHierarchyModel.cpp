#include "ActionHierarchyModel.h"
#include "ActionHierarchyModelItem.h"

#include <QDebug>
#include <QIcon>

namespace hdps
{

ActionHierarchyModel::ActionHierarchyModel(QObject* parent) :
    QAbstractItemModel(parent),
    _rootItem(new ActionHierarchyModelItem(nullptr))
{
}

ActionHierarchyModel::~ActionHierarchyModel()
{
    delete _rootItem;
}

QVariant ActionHierarchyModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    auto item = static_cast<ActionHierarchyModelItem*>(index.internalPointer());

    return item->getDataAtColumn(index.column(), role);
}

bool ActionHierarchyModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/)
{
    /*
    const auto column = static_cast<ActionHierarchyModelItem::Column>(index.column());

    auto ActionHierarchyModelItem = static_cast<ActionHierarchyModelItem*>((void*)index.internalPointer());
    
    switch (column) {
        case ActionHierarchyModelItem::Column::Name:
            ActionHierarchyModelItem->renameDataset(value.toString());
            break;

        case ActionHierarchyModelItem::Column::GUID:
            break;

        case ActionHierarchyModelItem::Column::Info:
            ActionHierarchyModelItem->getDataHierarchyItem()->setTaskDescription(value.toString());
            break;

        case ActionHierarchyModelItem::Column::Progress:
            ActionHierarchyModelItem->getDataHierarchyItem()->setTaskProgress(value.toFloat());
            break;

        case ActionHierarchyModelItem::Column::GroupIndex:
            ActionHierarchyModelItem->setGroupIndex(value.toInt());
            break;

        case ActionHierarchyModelItem::Column::IsGroup:
        case ActionHierarchyModelItem::Column::IsAnalyzing:
        case ActionHierarchyModelItem::Column::IsLocked:
            break;

        default:
            break;
    }

    emit dataChanged(index, index);
    */

    return true;
}

QModelIndex ActionHierarchyModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    ActionHierarchyModelItem* parentItem;

    if (!parent.isValid())
        parentItem = _rootItem;
    else
        parentItem = static_cast<ActionHierarchyModelItem*>(parent.internalPointer());

    auto childItem = parentItem->getChild(row);

    if (childItem)
        return createIndex(row, column, childItem);

    return QModelIndex();
}

QModelIndex ActionHierarchyModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
        return QModelIndex();

    auto childItem  = static_cast<ActionHierarchyModelItem*>(index.internalPointer());
    auto parentItem = childItem->getParent();

    if (parentItem == nullptr || parentItem == _rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int ActionHierarchyModel::rowCount(const QModelIndex& parent) const
{
    ActionHierarchyModelItem* parentItem;

    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = _rootItem;
    else
        parentItem = static_cast<ActionHierarchyModelItem*>(parent.internalPointer());

    return parentItem->getNumChildren();
}

int ActionHierarchyModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return static_cast<ActionHierarchyModelItem*>(parent.internalPointer())->getNumColumns();

    return _rootItem->getNumColumns();
}

ActionHierarchyModelItem* ActionHierarchyModel::getItem(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return nullptr;

    if (role != Qt::DisplayRole)
        return nullptr;

    return static_cast<ActionHierarchyModelItem*>(index.internalPointer());
}

Qt::ItemFlags ActionHierarchyModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    auto itemFlags = Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | QAbstractItemModel::flags(index);

    auto actionHierarchyModelItem = static_cast<ActionHierarchyModelItem*>(index.internalPointer());

    const auto itemIsLocked = actionHierarchyModelItem->getDataAtColumn(ActionHierarchyModelItem::Column::IsLocked, Qt::EditRole).toBool();

    if (!itemIsLocked && static_cast<ActionHierarchyModelItem::Column>(index.column()) == ActionHierarchyModelItem::Column::Name)
        itemFlags |= Qt::ItemIsEditable;

    if (!itemIsLocked && static_cast<ActionHierarchyModelItem::Column>(index.column()) == ActionHierarchyModelItem::Column::GroupIndex)
        itemFlags |= Qt::ItemIsEditable;

    return itemFlags;
}

QVariant ActionHierarchyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    /*
    if (orientation == Qt::Horizontal) {
        switch (role) {
            case Qt::DisplayRole:
            {
                switch (static_cast<ActionHierarchyModelItem::Column>(section))
                {
                    case ActionHierarchyModelItem::Column::Name:
                        return "Name";

                    case ActionHierarchyModelItem::Column::GUID:
                        return "ID";

                    case ActionHierarchyModelItem::Column::Info:
                        return "Info";

                    case ActionHierarchyModelItem::Column::Progress:
                        return "";

                    case ActionHierarchyModelItem::Column::GroupIndex:
                        return "Group ID";

                    case ActionHierarchyModelItem::Column::IsGroup:
                    case ActionHierarchyModelItem::Column::IsAnalyzing:
                    case ActionHierarchyModelItem::Column::IsLocked:
                        return "";

                    default:
                        break;
                }

                break;
            }

            case Qt::ToolTipRole:
            {
                const auto iconSize = QSize(14, 14);

                switch (static_cast<ActionHierarchyModelItem::Column>(section))
                {
                    case ActionHierarchyModelItem::Column::Name:
                        return "Name of the dataset";

                    case ActionHierarchyModelItem::Column::GUID:
                        return "Globally unique dataset identifier";

                    case ActionHierarchyModelItem::Column::Info:
                        return "Dataset additional information";

                    case ActionHierarchyModelItem::Column::Progress:
                        return "Task progress in percentage";

                    case ActionHierarchyModelItem::Column::GroupIndex:
                        return "Dataset group index";

                    case ActionHierarchyModelItem::Column::IsGroup:
                        return "Whether the dataset is composed of other datasets";

                    case ActionHierarchyModelItem::Column::IsAnalyzing:
                        return "Whether an analysis is taking place on the dataset";

                    case ActionHierarchyModelItem::Column::IsLocked:
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
    */

    return QVariant();
}

}
