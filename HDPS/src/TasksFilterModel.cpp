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
    _taskTypeFilterAction(this, "Task Type"),
    _taskStatusFilterAction(this, "Task Status", Task::statusNames.values(), Task::statusNames.values()),
    _statusTypeCounts()
{
    setRecursiveFilteringEnabled(true);

    _taskStatusFilterAction.setDefaultWidgetFlags(OptionsAction::ComboBox | OptionsAction::Selection);

    connect(&_taskTypeFilterAction, &OptionsAction::selectedOptionsChanged, this, &TasksFilterModel::invalidate);
    connect(&_taskStatusFilterAction, &OptionsAction::selectedOptionsChanged, this, &TasksFilterModel::invalidate);

    _taskTypeFilterAction.setToolTip("Filter tasks based on their type");
    _taskStatusFilterAction.setToolTip("Filter tasks based on their status");

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
    
    const auto taskTypes    = _taskTypeFilterAction.getSelectedOptions();
    const auto taskType     = getSourceData(index, TasksModel::Column::Type, Qt::DisplayRole).toString();

    if (!taskTypes.contains(taskType))
        return false;

    const auto taskStatusTypes  = _taskStatusFilterAction.getSelectedOptions();
    const auto statusIndex      = getSourceData(index, TasksModel::Column::Status, Qt::EditRole).toInt();
    const auto taskStatus       = Task::statusNames[static_cast<Task::Status>(statusIndex)];
    
    if (!taskStatusTypes.contains(taskStatus))
        return false;

    return true;
}

bool TasksFilterModel::lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const
{
    return lhs.data().toString() < rhs.data().toString();
}

void TasksFilterModel::setSourceModel(QAbstractItemModel* sourceModel)
{
    QSortFilterProxyModel::setSourceModel(sourceModel);

    connect(sourceModel, &QAbstractItemModel::rowsInserted, [this](const QModelIndex& parent, int first, int last) -> void {
        QTimer::singleShot(50, [this, parent, first, last]() -> void {
            auto selectedTaskTypes = _taskTypeFilterAction.getSelectedOptions();

            for (int row = first; row <= last; row++) {
                const auto status = this->sourceModel()->index(row, static_cast<int>(TasksModel::Column::Type), parent).data(Qt::DisplayRole).toString();

                if (_statusTypeCounts.contains(status)) {
                    _statusTypeCounts[status]++;
                }
                else {
                    _statusTypeCounts[status] = 1;

                    selectedTaskTypes << status;
                }
            }

            _taskTypeFilterAction.setOptions(_statusTypeCounts.keys());
            _taskTypeFilterAction.setSelectedOptions(selectedTaskTypes);
        });
    });

    connect(sourceModel, &QAbstractItemModel::rowsRemoved, [this](const QModelIndex& parent, int first, int last) -> void {
        QTimer::singleShot(50, [this, parent, first, last]() -> void {
            auto selectedTaskTypes = _taskTypeFilterAction.getSelectedOptions();

            for (int row = first; row <= last; row++) {
                const auto status = this->sourceModel()->index(row, static_cast<int>(TasksModel::Column::Type), parent).data(Qt::DisplayRole).toString();

                if (_statusTypeCounts.contains(status)) {
                    if (_statusTypeCounts[status] == 1)
                        _statusTypeCounts.remove(status);
                    else
                        _statusTypeCounts[status]--;
                }
            }

            _taskTypeFilterAction.setOptions(_statusTypeCounts.keys());
            _taskTypeFilterAction.setSelectedOptions(selectedTaskTypes);
        });
    });
}

QVariant TasksFilterModel::getSourceData(const QModelIndex& index, const TasksModel::Column& column, int role) const
{
    return sourceModel()->data(index.siblingAtColumn(static_cast<int>(column)), role);
}

}
