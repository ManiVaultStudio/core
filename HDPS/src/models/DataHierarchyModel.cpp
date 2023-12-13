// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DataHierarchyModel.h"
#include "DataHierarchyItem.h"
#include "DatasetsMimeData.h"
#include "Set.h"

#include <DataHierarchyItem.h>

#include <QDebug>
#include <QIcon>

namespace mv {

DataHierarchyModel::Item::Item(Dataset<DatasetImpl> dataset, bool editable /*= false*/) :
    QStandardItem(),
    QObject()
{
}

Dataset<DatasetImpl>& DataHierarchyModel::Item::getDataset()
{
    return _dataset;
}

const Dataset<DatasetImpl>& DataHierarchyModel::Item::getDataset() const
{
    return _dataset;
}

DataHierarchyModel::NameItem::NameItem(Dataset<DatasetImpl> dataset) :
    Item(dataset, true)
{
    connect(&getDataset(), &Dataset<DatasetImpl>::changed, this, &Item::refreshData);
    connect(&getDataset(), &Dataset<DatasetImpl>::guiNameChanged, this, &Item::refreshData);
}

QVariant DataHierarchyModel::NameItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getDataset().isValid() ? getDataset()->getGuiName() : "";

        case Qt::ToolTipRole:
            return QString("Dataset name: %1").arg(data(Qt::DisplayRole).toString());

        default:
            break;
    }

    return Item::data(role);
}

void DataHierarchyModel::NameItem::setData(const QVariant& value, int role /* = Qt::UserRole + 1 */)
{
    switch (role) {
        case Qt::EditRole:
        {
            if (getDataset().isValid())
                getDataset()->setText(value.toString());

            break;
        }

        default:
            Item::setData(value, role);
    }
}

DataHierarchyModel::IdItem::IdItem(Dataset<DatasetImpl> dataset) :
    Item(dataset)
{
    connect(getDataset().get(), &gui::WidgetAction::idChanged, this, [this](const QString& id) -> void {
        emitDataChanged();
    });
}

QVariant DataHierarchyModel::IdItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getDataset().isValid() ? getDataset()->getId() : "";

        case Qt::ToolTipRole:
            return "Dataset identifier: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

DataHierarchyModel::DataHierarchyModel(QObject* parent) :
    QStandardItemModel(parent)
{
}

Qt::DropActions DataHierarchyModel::supportedDragActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

//Qt::ItemFlags DataHierarchyModel::flags(const QModelIndex& index) const
//{
//    if (!index.isValid())
//        return Qt::NoItemFlags;
//
//    auto itemFlags = Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | QAbstractItemModel::flags(index);
//
//    auto dataHierarchyModelItem = static_cast<DataHierarchyModelItem*>(index.internalPointer());
//
//    const auto itemIsLocked = dataHierarchyModelItem->getDataAtColumn(DataHierarchyModelItem::Column::IsLocked, Qt::EditRole).toBool();
//
//    if (!itemIsLocked && static_cast<DataHierarchyModelItem::Column>(index.column()) == DataHierarchyModelItem::Column::Name)
//        itemFlags |= Qt::ItemIsEditable;
//
//    if (!itemIsLocked && static_cast<DataHierarchyModelItem::Column>(index.column()) == DataHierarchyModelItem::Column::GroupIndex)
//        itemFlags |= Qt::ItemIsEditable;
//
//    return itemFlags;
//}

QVariant DataHierarchyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (static_cast<Column>(section))
    {
        case Column::Name:
            return NameItem::headerData(orientation, role);

        case Column::ID:
            return IdItem::headerData(orientation, role);

        default:
            break;
    }

    return QVariant();
}

QMimeData* DataHierarchyModel::mimeData(const QModelIndexList& indexes) const
{
    Datasets datasets;

    for (const auto index : indexes)
        if (index.column() == 0)
            datasets << static_cast<Item*>(itemFromIndex(index))->getDataset();

    return new DatasetsMimeData(datasets);
}

void DataHierarchyModel::addDataHierarchyItem(DataHierarchyItem* dataHierarchyItem)
{
    Q_ASSERT(dataHierarchyItem != nullptr);

    if (!dataHierarchyItem)
        return;

    if (dataHierarchyItem->hasParent()) {

    }
    else {
        appendRow(Row(dataHierarchyItem->getDatasetReference()));
    }
}

//bool DataHierarchyModel::addDataHierarchyModelItem(const QModelIndex& parentModelIndex, mv::DataHierarchyItem& dataHierarchyItem)
//{
//    auto parentItem = !parentModelIndex.isValid() ? _rootItem : getItem(parentModelIndex, Qt::DisplayRole);
//
//    emit layoutAboutToBeChanged();
//    {
//        beginInsertRows(parentModelIndex, rowCount(parentModelIndex), rowCount(parentModelIndex) + 1);
//        {
//            parentItem->addChild(new DataHierarchyModelItem(&dataHierarchyItem));
//        }
//        endInsertRows();
//    }
//    emit layoutChanged();
//
//    for (auto child : dataHierarchyItem.getChildren())
//        addDataHierarchyModelItem(index(rowCount(parentModelIndex) - 1, 0, parentModelIndex), *child);
//
//    return true;
//}
//
//bool DataHierarchyModel::removeDataHierarchyModelItem(const QModelIndex& modelIndex)
//{
//    auto dataHierarchyModelItem = static_cast<DataHierarchyModelItem*>(modelIndex.internalPointer());
//
//    beginRemoveRows(modelIndex.parent(), modelIndex.row(), modelIndex.row());
//    {
//        delete dataHierarchyModelItem;
//    }
//    endRemoveRows();
//
//    return true;
//}

DataHierarchyModel::Row::Row(Dataset<DatasetImpl> dataset) :
    QList<QStandardItem*>()
{
    append(new NameItem(dataset));
    append(new IdItem(dataset));
}

}



//QVariant DataHierarchyModel::data(const QModelIndex& index, int role) const
//{
//    if (!index.isValid())
//        return QVariant();
//
//    auto item = static_cast<DataHierarchyModelItem*>(index.internalPointer());
//
//    return item->getDataAtColumn(index.column(), role);
//}
//
//bool DataHierarchyModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/)
//{
//    const auto column = static_cast<DataHierarchyModelItem::Column>(index.column());
//
//    auto dataHierarchyModelItem = static_cast<DataHierarchyModelItem*>((void*)index.internalPointer());
//    
//    switch (column) {
//        case DataHierarchyModelItem::Column::Name:
//            dataHierarchyModelItem->renameDataset(value.toString());
//            break;
//
//        case DataHierarchyModelItem::Column::GUID:
//        case DataHierarchyModelItem::Column::Info:
//        case DataHierarchyModelItem::Column::Progress:
//            break;
//
//        case DataHierarchyModelItem::Column::GroupIndex:
//            dataHierarchyModelItem->setGroupIndex(value.toInt());
//            break;
//
//        case DataHierarchyModelItem::Column::IsGroup:
//        case DataHierarchyModelItem::Column::IsAnalyzing:
//        case DataHierarchyModelItem::Column::IsLocked:
//            break;
//
//        default:
//            break;
//    }
//
//    emit dataChanged(index, index);
//
//    return true;
//}
//

//if (orientation == Qt::Horizontal) {
//    switch (role) {
//        case Qt::DisplayRole:
//        {
//            switch (static_cast<Column>(section))
//            {
//                case Column::Progress:
//                    return "Progress";
//
//                default:
//                    break;
//            }
//
//            break;
//        }
//
//        case Qt::EditRole:
//        {
//            switch (static_cast<Column>(section))
//            {
//                case Column::Progress:
//                    return "Progress";
//
//                case Column::GroupIndex:
//                    return "Group ID";
//
//                case Column::IsGroup:
//                    return "Is group";
//
//                case Column::IsAnalyzing:
//                    return "Is analyzing";
//
//                case Column::IsLocked:
//                    return "Is locked";
//
//                default:
//                    break;
//            }
//
//            break;
//        }
//
//        case Qt::ToolTipRole:
//        {
//            const auto iconSize = QSize(14, 14);
//
//            switch (static_cast<Column>(section))
//            {
//                case Column::Progress:
//                    return "Task progress";
//
//                case Column::GroupIndex:
//                    return "Dataset group index";
//
//                case Column::IsGroup:
//                    return "Whether the dataset is composed of other datasets";
//
//                case Column::IsAnalyzing:
//                    return "Whether an analysis is taking place on the dataset";
//
//                case Column::IsLocked:
//                    return "Whether the dataset is locked";
//
//                default:
//                    break;
//            }
//
//            break;
//        }
//
//        default:
//            break;
//    }
//}