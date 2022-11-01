#include "ActionHierarchyModel.h"
#include "ActionHierarchyModelItem.h"

#include <QDebug>

namespace hdps
{

ActionHierarchyModel::ActionHierarchyModel(QObject* parent, gui::WidgetAction* rootAction) :
    QAbstractItemModel(parent),
    _rootItem(new ActionHierarchyModelItem(rootAction, nullptr))
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
    auto actionHierarchyModelItem = static_cast<ActionHierarchyModelItem*>((void*)index.internalPointer());
    
    QModelIndexList childIndices;

    const auto nameIndex = index.siblingAtColumn(ActionHierarchyModelItem::Column::Name);

    for (int rowIndex = 0; rowIndex < rowCount(nameIndex); ++rowIndex)
        childIndices << this->index(rowIndex, index.column(), nameIndex);

    auto action = actionHierarchyModelItem->getAction();

    switch (role) {
        case Qt::CheckStateRole:
        {
            switch (index.column()) {
                case ActionHierarchyModelItem::Column::Name:
                {
                    action->setEnabled(value.toBool());

                    for (const auto& childIndex : childIndices)
                        setData(childIndex, value.toBool(), Qt::CheckStateRole);

                    break;
                }

                case ActionHierarchyModelItem::Column::Visible:
                {
                    action->setVisible(value.toBool());
                    
                    for (const auto& childIndex : childIndices)
                        setData(childIndex, value.toBool(), Qt::CheckStateRole);

                    break;
                }

                case ActionHierarchyModelItem::Column::MayPublish:
                {
                    action->setConnectionPermissions(WidgetAction::PublishViaGui, !value.toBool());

                    for (const auto& childIndex : childIndices)
                        setData(childIndex, value.toBool(), Qt::CheckStateRole);

                    break;
                }

                case ActionHierarchyModelItem::Column::MayConnect:
                {
                    action->setConnectionPermissions(WidgetAction::ConnectViaGui, !value.toBool());

                    for (const auto& childIndex : childIndices)
                        setData(childIndex, value.toBool(), Qt::CheckStateRole);

                    break;
                }

                case ActionHierarchyModelItem::Column::MayDisconnect:
                {
                    action->setConnectionPermissions(WidgetAction::DisconnectViaGui, !value.toBool());

                    for (const auto& childIndex : childIndices)
                        setData(childIndex, value.toBool(), Qt::CheckStateRole);

                    break;
                }

                default:
                    break;
            }

            break;
        }
    }

    emit dataChanged(index, index);

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

    auto action = static_cast<ActionHierarchyModelItem*>(index.internalPointer())->getAction();

    switch (index.column())
    {
        case ActionHierarchyModelItem::Column::Name:
            itemFlags |= Qt::ItemIsUserCheckable;
            break;

        //case ActionHierarchyModelItem::Column::Visible:
        //{
        //    if (!action->isVisible())
        //        itemFlags |= Qt::ItemIsSelectable;
        //    
        //    break;
        //}

        //case ActionHierarchyModelItem::Column::MayPublish:
        //{
        //    if (!action->mayPublish(WidgetAction::Gui))
        //        itemFlags |= Qt::ItemIsSelectable;

        //    break;
        //}

        //case ActionHierarchyModelItem::Column::MayConnect:
        //{
        //    if (!action->mayConnect(WidgetAction::Gui))
        //        itemFlags |= Qt::ItemIsSelectable;

        //    break;
        //}

        //case ActionHierarchyModelItem::Column::MayDisconnect:
        //{
        //    if (!action->mayDisconnect(WidgetAction::Gui))
        //        itemFlags |= Qt::ItemIsSelectable;

        //    break;
        //}

        default:
            break;
    }

    return itemFlags;
}

QVariant ActionHierarchyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        switch (role) {
            case Qt::DisplayRole:
            {
                switch (static_cast<ActionHierarchyModelItem::Column>(section))
                {
                    case ActionHierarchyModelItem::Column::Name:
                        return "Name";

                    case ActionHierarchyModelItem::Column::Visible:
                        return "";

                    case ActionHierarchyModelItem::Column::MayPublish:
                        return "";

                    case ActionHierarchyModelItem::Column::MayConnect:
                        return "";

                    case ActionHierarchyModelItem::Column::MayDisconnect:
                        return "";

                    default:
                        break;
                }

                break;
            }

            case Qt::ToolTipRole:
            {
                switch (static_cast<ActionHierarchyModelItem::Column>(section))
                {
                    case ActionHierarchyModelItem::Column::Name:
                        return "Name of the action";

                    case ActionHierarchyModelItem::Column::Visible:
                        return "Whether the action is visible or not";

                    case ActionHierarchyModelItem::Column::MayPublish:
                        return "Whether the action may be published or not";

                    case ActionHierarchyModelItem::Column::MayConnect:
                        return "Whether the action may connect to a public action or not";

                    case ActionHierarchyModelItem::Column::MayDisconnect:
                        return "Whether the action may disconnect from a public action or not";

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

QModelIndexList ActionHierarchyModel::fetchModelIndices(QModelIndex modelIndex /*= QModelIndex()*/, bool childrenOnly /*= false*/) const
{
    QModelIndexList modelIndexList;

    if (!modelIndex.isValid())
        return modelIndexList;

    if (!childrenOnly)
        modelIndexList << modelIndex;

    for (int rowIndex = 0; rowIndex < rowCount(modelIndex); ++rowIndex) {
        QModelIndex index = this->index(rowIndex, 0, modelIndex);

        modelIndexList << index;

        if (hasChildren(index))
            modelIndexList << fetchModelIndices(index, false);
    }

    return modelIndexList;
}

}
