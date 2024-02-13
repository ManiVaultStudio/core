// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SelectionsModel.h"

#include "models/AbstractDataHierarchyModel.h"

#include "util/Exception.h"

#include <QDebug>

#ifdef _DEBUG
    #define SELECTIONS_MODEL_VERBOSE
#endif

namespace mv {

using namespace util;

SelectionsModel::SelectionItem::SelectionItem(Dataset<DatasetImpl> selection) :
    QStandardItem(),
    _selection(selection)
{
    Q_ASSERT(_selection.isValid());
}

Dataset<DatasetImpl> SelectionsModel::SelectionItem::getSelection() const
{
    return _selection;
}

QVariant SelectionsModel::SelectionNameItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getSelection()->getGuiName();

        case Qt::ToolTipRole:
            return "Selection dataset name: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return {};
}

SelectionsModel::Row::Row(Dataset<DatasetImpl> selection) :
    QList<QStandardItem*>()
{
    append(new SelectionsModel::SelectionNameItem(selection));
}

SelectionsModel::SelectionsModel(QObject* parent) :
    QStandardItemModel(parent),
    _countAction(this, "Number of selections")
{
    setHorizontalHeaderLabels({ "Name", "ID", "Raw data name" });

    _countAction.setToolTip("Number of selections");
    _countAction.setEnabled(false);

    connect(&mv::data(), &AbstractDataManager::selectionAdded, this, &SelectionsModel::populateFromDataManager);
    connect(&mv::data(), &AbstractDataManager::selectionRemoved, this, &SelectionsModel::populateFromDataManager);

    populateFromDataManager();
}

QVariant SelectionsModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    switch (static_cast<Column>(section))
    {
        case Column::Name:
            return SelectionsModel::SelectionNameItem::headerData(orientation, role);

        default:
            break;
    }

    return {};
}

void SelectionsModel::populateFromDataManager()
{
#ifdef SELECTIONS_MODEL_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    setRowCount(0);

    for (const auto& selection : mv::data().getAllSelections())
        appendRow(Row(selection));

    if (rowCount() == 1)
        _countAction.setString(QString("%1 selection").arg(rowCount()));
    else
        _countAction.setString(QString("%1 selections").arg(rowCount()));
}

}