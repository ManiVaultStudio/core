// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DataHierarchyModel.h"
#include "DataHierarchyItem.h"
#include "DatasetsMimeData.h"
#include "Set.h"

#include "DataHierarchyItem.h"
#include "util/Exception.h"

#include <QDebug>
#include <QIcon>

namespace mv {

using namespace util;

DataHierarchyModel::Item::Item(Dataset<DatasetImpl> dataset, bool editable /*= false*/) :
    QStandardItem(),
    QObject(),
    _dataset(dataset)
{
    Q_ASSERT(_dataset.isValid());

    connect(&getDataset()->getDataHierarchyItem(), &DataHierarchyItem::lockedChanged, this, [this](bool locked) -> void {
        emitDataChanged();
    });
}

QVariant DataHierarchyModel::Item::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::ForegroundRole:
        {
            if (_dataset.isValid())
                break;

            return _dataset->isLocked() ? QApplication::palette().color(QPalette::Disabled, QPalette::Text).name() : QApplication::palette().color(QPalette::Normal, QPalette::Text).name();
        }

        default:
            break;
    }

    return QStandardItem::data(role);
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
    connect(&getDataset(), &Dataset<DatasetImpl>::guiNameChanged, this, [this]() -> void {
        emitDataChanged();
    });
}

QVariant DataHierarchyModel::NameItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getDataset().isValid() ? getDataset()->getGuiName() : "";

        case Qt::ToolTipRole:
            return QString("Dataset name: %1").arg(data(Qt::DisplayRole).toString());

        case Qt::DecorationRole:
        {
            if (!getDataset().isValid())
                break;

            return getDataset()->getDataHierarchyItem().getIcon();
        }

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

DataHierarchyModel::DatasetIdItem::DatasetIdItem(Dataset<DatasetImpl> dataset) :
    Item(dataset)
{
    connect(getDataset().get(), &gui::WidgetAction::idChanged, this, [this](const QString& id) -> void {
        emitDataChanged();
    });
}

QVariant DataHierarchyModel::DatasetIdItem::data(int role /*= Qt::UserRole + 1*/) const
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

DataHierarchyModel::ProgressItem::ProgressItem(Dataset<DatasetImpl> dataset) :
    Item(dataset),
    _taskAction(this, "Task")
{
    _taskAction.setTask(&getDatasetTask());

    connect(&getDatasetTask(), &Task::progressChanged, this, [this]() -> void {
        emitDataChanged();
    });

    connect(&getDatasetTask(), &Task::progressDescriptionChanged, this, [this]() -> void {
        emitDataChanged();
    });

    connect(&getDatasetTask(), &Task::statusChanged, this, [this]() -> void {
        emitDataChanged();
    });
}

QVariant DataHierarchyModel::ProgressItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return getDataset().isValid() ? const_cast<Dataset<DatasetImpl>&>(getDataset())->getTask().getProgress() : .0f;

        case Qt::DisplayRole:
            break;

        case Qt::ToolTipRole:
            return "Dataset task progress: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

QWidget* DataHierarchyModel::ProgressItem::createDelegateEditorWidget(QWidget* parent)
{
    return _taskAction.getProgressAction().createWidget(parent);
}

DataHierarchyModel::GroupIndexItem::GroupIndexItem(Dataset<DatasetImpl> dataset) :
    Item(dataset)
{
}

QVariant DataHierarchyModel::GroupIndexItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return getDataset().isValid() ? getDataset()->getGroupIndex() : -1;

        case Qt::DisplayRole:
            return QString::number(data(Qt::EditRole).toInt());

        case Qt::ToolTipRole:
            return "Dataset group index: " + data(Qt::DisplayRole).toString();

        case Qt::TextAlignmentRole:
            return static_cast<std::int32_t>(Qt::AlignVCenter | Qt::AlignRight);

        default:
            break;
    }

    return Item::data(role);
}

void DataHierarchyModel::GroupIndexItem::setData(const QVariant& value, int role /* = Qt::UserRole + 1 */)
{
    switch (role) {
        case Qt::EditRole:
        {
            if (getDataset().isValid())
                getDataset()->setGroupIndex(value.toInt());

            break;
        }

        default:
            Item::setData(value, role);
    }
}

DataHierarchyModel::IsGroupItem::IsGroupItem(Dataset<DatasetImpl> dataset) :
    Item(dataset)
{
}

QVariant DataHierarchyModel::IsGroupItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        {
            if (!getDataset().isValid())
                break;

            return getDataset()->isProxy();
        }

        case Qt::DisplayRole:
            return data(Qt::EditRole).toBool() ? "yes" : "no";

        case Qt::ToolTipRole:
            return "Dataset is a group: " + data(Qt::DisplayRole).toString();

        case Qt::DecorationRole:
        {
            if (!getDataset().isValid())
                break;

            if (getDataset()->isProxy())
                return Application::getIconFont("FontAwesome").getIcon("object-group");
        }

        default:
            break;
    }

    return Item::data(role);
}

DataHierarchyModel::IsLockedItem::IsLockedItem(Dataset<DatasetImpl> dataset) :
    Item(dataset)
{
}

QVariant DataHierarchyModel::IsLockedItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        {
            if (!getDataset().isValid())
                break;

            return getDataset()->isLocked();
        }

        case Qt::DisplayRole:
            break;

        case Qt::ToolTipRole:
            return "Dataset is a locked: " + data(Qt::DisplayRole).toString();

        case Qt::DecorationRole:
            return data(Qt::EditRole).toBool() ? Application::getIconFont("FontAwesome").getIcon("lock") : QIcon();

        default:
            break;
    }

    return Item::data(role);
}

DataHierarchyModel::Row::Row(Dataset<DatasetImpl> dataset) :
    QList<QStandardItem*>()
{
    append(new NameItem(dataset));
    append(new DatasetIdItem(dataset));
    append(new ProgressItem(dataset));
    append(new GroupIndexItem(dataset));
    append(new IsGroupItem(dataset));
    append(new IsLockedItem(dataset));
}

DataHierarchyModel::DataHierarchyModel(QObject* parent) :
    QStandardItemModel(parent)
{
    setColumnCount(static_cast<int>(Column::Count));

    connect(&dataHierarchy(), &AbstractDataHierarchyManager::itemAdded, this, [this](DataHierarchyItem& dataHierarchyItem) -> void {
        addDataHierarchyItem(dataHierarchyItem);
    });

    connect(&dataHierarchy(), &AbstractDataHierarchyManager::itemAboutToBeRemoved, this, [this](const Dataset<DatasetImpl>& dataset) {
        //removeDataHierarchyItem(dataset->getDataHierarchyItem());
    });

    for (const auto topLevelItem : dataHierarchy().getTopLevelItems())
        addDataHierarchyItem(*topLevelItem);
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

        case Column::DatasetId:
            return DatasetIdItem::headerData(orientation, role);

        case Column::Progress:
            return ProgressItem::headerData(orientation, role);

        case Column::GroupIndex:
            return GroupIndexItem::headerData(orientation, role);

        case Column::IsGroup:
            return IsGroupItem::headerData(orientation, role);

        case Column::IsLocked:
            return IsLockedItem::headerData(orientation, role);

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

void DataHierarchyModel::addDataHierarchyItem(DataHierarchyItem& dataHierarchyItem)
{
    try {
        if (dataHierarchyItem.hasParent()) {
            const auto matches = match(index(0, static_cast<int>(Column::DatasetId), QModelIndex()), Qt::EditRole, dataHierarchyItem.getParent().getDataset()->getId(), -1, Qt::MatchFlag::MatchExactly | Qt::MatchFlag::MatchRecursive);

            if (matches.isEmpty())
                throw std::runtime_error("Parent data hierarchy item not found in model");

            itemFromIndex(matches.first().siblingAtColumn(static_cast<int>(Column::Name)))->appendRow(Row(dataHierarchyItem.getDatasetReference()));
        }
        else {
            appendRow(Row(dataHierarchyItem.getDatasetReference()));
        }
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to add data hierarchy item to the data hierarchy model", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to add data hierarchy item to the data hierarchy model");
    }
}

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

}
