// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TasksFilterModel.h"

#include <QDebug>

using namespace hdps::gui;

namespace hdps
{

TasksFilterModel::TasksFilterModel(QObject* parent /*= nullptr*/) :
    QSortFilterProxyModel(parent),
    _typeFilterAction(this, "Type"),
    _statusFilterAction(this, "Status", Task::statusNames.values(), Task::statusNames.values())
{
    setRecursiveFilteringEnabled(true);

    _statusFilterAction.setDefaultWidgetFlags(OptionsAction::ComboBox | OptionsAction::Selection);

    connect(&_typeFilterAction, &StringAction::stringChanged, this, &TasksFilterModel::invalidate);
    connect(&_statusFilterAction, &OptionsAction::selectedOptionsChanged, this, &TasksFilterModel::invalidate);

    _typeFilterAction.setToolTip("Filter tasks based on their type");
    _statusFilterAction.setToolTip("Filter tasks based on their status");

    invalidate();
}

bool TasksFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    const auto index = sourceModel()->index(row, 0, parent);

    if (!index.isValid())
        return true;

    if (filterRegularExpression().isValid()) {
        const auto key = getSourceData(index, static_cast<TasksModel::Column>(filterKeyColumn()), filterRole()).toString();

        if (!key.contains(filterRegularExpression()))
            return false;
    }
    
    const auto typeFilter = _typeFilterAction.getString();

    if (!typeFilter.isEmpty()) {
        const auto type = getSourceData(index, TasksModel::Column::Type, Qt::EditRole).toString();

        if (type != typeFilter)
            return false;
    }

    if (_statusFilterAction.hasSelectedOptions()) {
        const auto selectedOptions  = _statusFilterAction.getSelectedOptions();
        const auto status           = getSourceData(index, TasksModel::Column::Status, Qt::DisplayRole).toString();

        if (!selectedOptions.contains(status))
            return false;
    }
    

    return true;
}

bool TasksFilterModel::lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const
{
    return lhs.data().toString() < rhs.data().toString();
}

QVariant TasksFilterModel::getSourceData(const QModelIndex& index, const TasksModel::Column& column, int role) const
{
    return sourceModel()->data(index.siblingAtColumn(static_cast<int>(column)), role);
}

}
