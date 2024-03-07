// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DataHierarchyTreeModel.h"
#include "DataHierarchyItem.h"
#include "DatasetsMimeData.h"
#include "Set.h"

#include "util/Exception.h"

#include <QDebug>

#ifdef _DEBUG
    #define DATA_HIERARCHY_TREE_MODEL_VERBOSE
#endif

namespace mv {

using namespace util;

DataHierarchyTreeModel::DataHierarchyTreeModel(QObject* parent) :
    AbstractDataHierarchyModel(parent)
{
    connect(&dataHierarchy(), &AbstractDataHierarchyManager::itemAdded, this, &DataHierarchyTreeModel::addDataHierarchyModelItem);
    connect(&dataHierarchy(), &AbstractDataHierarchyManager::itemAboutToBeRemoved, this, &DataHierarchyTreeModel::removeDataHierarchyModelItem);
    connect(&dataHierarchy(), &AbstractDataHierarchyManager::itemParentChanged, this, &DataHierarchyTreeModel::reparentDataHierarchyModelItem);

    populateFromDataHierarchyManager();
}

Qt::DropActions DataHierarchyTreeModel::supportedDragActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

void DataHierarchyTreeModel::populateFromDataHierarchyManager()
{
#ifdef DATA_HIERARCHY_TREE_MODEL_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    for (const auto topLevelDataHierarchyItem : dataHierarchy().getTopLevelItems())
        addDataHierarchyModelItem(topLevelDataHierarchyItem);
}

void DataHierarchyTreeModel::addDataHierarchyModelItem(DataHierarchyItem* dataHierarchyItem)
{
    try {

        if (!dataHierarchyItem)
            throw std::runtime_error("Data hierarchy item pointer is invalid");

        if (!match(index(0, static_cast<int>(Column::DatasetId), QModelIndex()), Qt::EditRole, dataHierarchyItem->getDataset()->getId(), -1, Qt::MatchFlag::MatchExactly | Qt::MatchFlag::MatchRecursive).isEmpty())
            return;

#ifdef DATA_HIERARCHY_TREE_MODEL_VERBOSE
        qDebug() << "Add dataset" << dataHierarchyItem->getDataset()->getGuiName() << "to the data hierarchy tree model";
#endif

        if (dataHierarchyItem->hasParent()) {
            const auto matches = match(index(0, static_cast<int>(Column::DatasetId), QModelIndex()), Qt::EditRole, dataHierarchyItem->getParent()->getDataset()->getId(), -1, Qt::MatchFlag::MatchExactly | Qt::MatchFlag::MatchRecursive);

            if (matches.isEmpty())
                throw std::runtime_error("Parent data hierarchy item not found in model");

            itemFromIndex(matches.first().siblingAtColumn(static_cast<int>(Column::Name)))->appendRow(Row(dataHierarchyItem->getDataset()));
        }
        else {
            appendRow(Row(dataHierarchyItem->getDataset()));
        }

        for (auto childDataHierarchyItem : dataHierarchyItem->getChildren(true))
            addDataHierarchyModelItem(childDataHierarchyItem);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to add item to the data hierarchy tree model", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to add item to the data hierarchy tree model");
    }
}

void DataHierarchyTreeModel::removeDataHierarchyModelItem(DataHierarchyItem* dataHierarchyItem)
{
    try {

        if (!dataHierarchyItem)
            throw std::runtime_error("Data hierarchy item pointer is invalid");

        const auto matches = match(index(0, static_cast<int>(Column::DatasetId), QModelIndex()), Qt::EditRole, dataHierarchyItem->getDataset()->getId(), -1, Qt::MatchFlag::MatchExactly | Qt::MatchFlag::MatchRecursive);

        if (matches.isEmpty())
            throw std::runtime_error(QString("%1 not found in model").arg(dataHierarchyItem->getDataset()->getGuiName()).toStdString());

        auto item = itemFromIndex(matches.first().siblingAtColumn(static_cast<int>(Column::Name)));

        if (!item)
            throw std::runtime_error("QStandardItemModel::itemFromIndex() returned nullptr");

#ifdef DATA_HIERARCHY_TREE_MODEL_VERBOSE
        qDebug() << "Remove dataset" << dataHierarchyItem->getDataset()->getGuiName() << "from the data hierarchy tree model";
#endif

        bool removeRowsResult = false;

        if (item->parent())
            removeRowsResult = removeRows(item->row(), 1, item->parent()->index());
        else
            removeRowsResult = removeRows(item->row(), 1);

        if (!removeRowsResult)
            throw std::runtime_error("QStandardItemModel::removeRows() failed");
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to remove item from the data hierarchy tree model", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to remove item from the data hierarchy tree model");
    }
}

void DataHierarchyTreeModel::reparentDataHierarchyModelItem(DataHierarchyItem* dataHierarchyItem)
{
    try {

        if (!dataHierarchyItem)
            throw std::runtime_error("Data hierarchy item pointer is invalid");

#ifdef DATA_HIERARCHY_TREE_MODEL_VERBOSE
        qDebug() << "Re-parent dataset" << dataHierarchyItem->getDataset()->getGuiName();
#endif

        removeDataHierarchyModelItem(dataHierarchyItem);
        addDataHierarchyModelItem(dataHierarchyItem);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to re-parent data hierarchy model item", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to re-parent data hierarchy model item");
    }
}

}
