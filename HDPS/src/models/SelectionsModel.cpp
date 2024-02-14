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

SelectionsModel::Item::Item(Dataset<DatasetImpl> selection) :
    QStandardItem(),
    _selection(selection)
{
    Q_ASSERT(_selection.isValid());
}

Dataset<DatasetImpl> SelectionsModel::Item::getSelection() const
{
    return _selection;
}

QVariant SelectionsModel::NameItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getSelection()->getGuiName();

        case Qt::ToolTipRole:
            return "Name: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return {};
}

QVariant SelectionsModel::IdItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getSelection()->getId();

        case Qt::ToolTipRole:
            return "ID: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return {};
}

QVariant SelectionsModel::RawDataNameItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getSelection()->getRawDataName();

        case Qt::ToolTipRole:
            return "Raw data name: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return {};
}

QVariant SelectionsModel::RawDataTypeItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return mv::data().getRawDataType(getSelection()->getRawDataName());

        case Qt::ToolTipRole:
            return "Type: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return {};
}

SelectionsModel::Row::Row(Dataset<DatasetImpl> selection) :
    QList<QStandardItem*>()
{
    append(new NameItem(selection));
    append(new IdItem(selection));
    append(new RawDataNameItem(selection));
    append(new RawDataTypeItem(selection));
}

SelectionsModel::SelectionsModel(QObject* parent) :
    QStandardItemModel(parent),
    _countAction(this, "Number of selections")
{
    setColumnCount(static_cast<int>(Column::Count));

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
            return NameItem::headerData(orientation, role);

        case Column::ID:
            return IdItem::headerData(orientation, role);

        case Column::RawDataName:
            return RawDataNameItem::headerData(orientation, role);

        case Column::RawDataType:
            return RawDataTypeItem::headerData(orientation, role);

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

    _countAction.setString(QString("Count: %1").arg(QString::number(rowCount())));
}

}