// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DataHierarchyListModel.h"
#include "DataHierarchyItem.h"
#include "DatasetsMimeData.h"
#include "Set.h"

#include "util/Exception.h"

#include <QDebug>

#ifdef _DEBUG
    //#define DATA_HIERARCHY_LIST_MODEL_VERBOSE
#endif

namespace mv {

using namespace util;

DataHierarchyListModel::DataHierarchyListModel(QObject* parent) :
    AbstractDataHierarchyModel(parent)
{
    connect(&dataHierarchy(), &AbstractDataHierarchyManager::itemAdded, this, &DataHierarchyListModel::addDataHierarchyModelItem);
    connect(&dataHierarchy(), &AbstractDataHierarchyManager::itemAboutToBeRemoved, this, &DataHierarchyListModel::removeDataHierarchyModelItem);

    populateFromDataHierarchyManager();
}

Qt::DropActions DataHierarchyListModel::supportedDragActions() const
{
    return Qt::IgnoreAction;
}

Qt::DropActions DataHierarchyListModel::supportedDropActions() const
{
    return Qt::IgnoreAction;
}

void DataHierarchyListModel::populateFromDataHierarchyManager()
{
#ifdef DATA_HIERARCHY_LIST_MODEL_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    for (const auto topLevelDataHierarchyItem : dataHierarchy().getTopLevelItems())
        addDataHierarchyModelItem(topLevelDataHierarchyItem);
}

void DataHierarchyListModel::addDataHierarchyModelItem(DataHierarchyItem* dataHierarchyItem)
{
    try {

        if (!dataHierarchyItem)
            throw std::runtime_error("Data hierarchy item pointer is invalid");

        if (!match(index(0, static_cast<int>(Column::DatasetId), QModelIndex()), Qt::EditRole, dataHierarchyItem->getDataset()->getId(), -1, Qt::MatchFlag::MatchExactly | Qt::MatchFlag::MatchRecursive).isEmpty())
            return;

#ifdef DATA_HIERARCHY_LIST_MODEL_VERBOSE
        qDebug() << "Add dataset" << dataHierarchyItem->getDataset()->getGuiName() << "to the data hierarchy list model";
#endif

        appendRow(Row(dataHierarchyItem->getDataset()));

        for (auto childDataHierarchyItem : dataHierarchyItem->getChildren(true))
            addDataHierarchyModelItem(childDataHierarchyItem);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to add item to the data hierarchy list model", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to add item to the data hierarchy list model");
    }
}

void DataHierarchyListModel::removeDataHierarchyModelItem(DataHierarchyItem* dataHierarchyItem)
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

#ifdef DATA_HIERARCHY_LIST_MODEL_VERBOSE
        qDebug() << "Remove dataset" << dataHierarchyItem->getDataset()->getGuiName() << "from the data hierarchy list model";
#endif

        if (!removeRows(item->row(), 1))
            throw std::runtime_error("QStandardItemModel::removeRows() failed");
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to remove item from the data hierarchy list model", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to remove item from the data hierarchy list model");
    }
}

}
