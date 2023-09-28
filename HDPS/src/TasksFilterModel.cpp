// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TasksFilterModel.h"

#include <QDebug>

#ifdef _DEBUG
    #define TASKS_FILTER_MODEL_VERBOSE
#endif

using namespace hdps::gui;

namespace hdps
{

TasksFilterModel::TasksFilterModel(QObject* parent /*= nullptr*/) :
    QSortFilterProxyModel(parent),
    _taskTypeFilterAction(this, "Type"),
    _taskScopeFilterAction(this, "Scope", Task::scopeNames.values(), Task::scopeNames.values()),
    _taskStatusFilterAction(this, "Status", Task::statusNames.values(), Task::statusNames.values()),
    _hideDisabledTasksFilterAction(this, "Hide disabled tasks", true),
    _hideHiddenTasksFilterAction(this, "Hide hidden tasks", true),
    _parentTaskFilterAction(this, "Parent task identifier"),
    _statusTypeCounts(),
    _rowCount(0)
{
    setDynamicSortFilter(true);
    setRecursiveFilteringEnabled(true);

    _taskStatusFilterAction.setDefaultWidgetFlags(OptionsAction::ComboBox | OptionsAction::Selection);

    connect(&_taskTypeFilterAction, &OptionsAction::selectedOptionsChanged, this, &TasksFilterModel::invalidate);
    connect(&_taskScopeFilterAction, &OptionsAction::selectedOptionsChanged, this, &TasksFilterModel::invalidate);
    connect(&_taskStatusFilterAction, &OptionsAction::selectedOptionsChanged, this, &TasksFilterModel::invalidate);
    connect(&_hideDisabledTasksFilterAction, &ToggleAction::toggled, this, &TasksFilterModel::invalidate);
    connect(&_hideHiddenTasksFilterAction, &ToggleAction::toggled, this, &TasksFilterModel::invalidate);
    connect(&_parentTaskFilterAction, &StringAction::stringChanged, this, &TasksFilterModel::invalidate);

    _taskTypeFilterAction.setToolTip("Filter tasks based on their type");
    _taskScopeFilterAction.setToolTip("Filter tasks based on their scope");
    _taskStatusFilterAction.setToolTip("Filter tasks based on their status");
    _hideDisabledTasksFilterAction.setToolTip("Hide or show disabled tasks");
    _hideHiddenTasksFilterAction.setToolTip("Hide or show hidden tasks");
    _parentTaskFilterAction.setToolTip("Show only tasks of which the parent identifier matches");

    invalidate();
}

bool TasksFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    const auto index = sourceModel()->index(row, 0, parent);

    if (!index.isValid())
        return true;

    if (filterRegularExpression().isValid()) {
        const auto key = getSourceData(index, static_cast<AbstractTasksModel::Column>(filterKeyColumn()), filterRole()).toString();

        if (!key.contains(filterRegularExpression()))
            return false;
    }
    
    const auto taskTypes    = _taskTypeFilterAction.getSelectedOptions();
    const auto taskType     = getSourceData(index, AbstractTasksModel::Column::Type, Qt::DisplayRole).toString();

    if (!taskTypes.contains(taskType))
        return false;

    const auto taskScopes   = _taskScopeFilterAction.getSelectedOptions();
    const auto taskScope    = getSourceData(index, AbstractTasksModel::Column::Scope, Qt::DisplayRole).toString();

    if (!taskScopes.contains(taskScope))
        return false;

    const auto taskStatusTypes  = _taskStatusFilterAction.getSelectedOptions();
    const auto statusIndex      = getSourceData(index, AbstractTasksModel::Column::Status, Qt::EditRole).toInt();
    const auto taskStatus       = Task::statusNames[static_cast<Task::Status>(statusIndex)];
    
    if (!taskStatusTypes.contains(taskStatus))
        return false;

    const auto taskIsEnabled = getSourceData(index, AbstractTasksModel::Column::Enabled, Qt::EditRole).toBool();

    if (_hideDisabledTasksFilterAction.isChecked() && !taskIsEnabled)
        return false;

    const auto taskIsVisible = getSourceData(index, AbstractTasksModel::Column::Visible, Qt::EditRole).toBool();

    if (_hideHiddenTasksFilterAction.isChecked() && !taskIsVisible)
        return false;

    const auto filterParentTask = _parentTaskFilterAction.getString();
    const auto parentTask       = getSourceData(index, AbstractTasksModel::Column::ParentID, Qt::EditRole).toString();

    if (!filterParentTask.isEmpty())
        if (parentTask != filterParentTask)
            return false;

    return true;
}

bool TasksFilterModel::lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const
{
    return lhs.data().toString() < rhs.data().toString();
}

void TasksFilterModel::setSourceModel(QAbstractItemModel* sourceModel)
{
    Q_ASSERT(sourceModel != nullptr);

    if (sourceModel == nullptr)
        return;

#ifdef TASKS_FILTER_MODEL_VERBOSE
    qDebug() <<  this << __FUNCTION__ << sourceModel;
#endif

    QSortFilterProxyModel::setSourceModel(sourceModel);

    for (int row = 0; row < sourceModel->rowCount(); row++) {
        const auto status = this->sourceModel()->index(row, static_cast<int>(AbstractTasksModel::Column::Type)).data(Qt::DisplayRole).toString();

        if (_statusTypeCounts.contains(status)) {
            _statusTypeCounts[status]++;
        }
        else {
            _statusTypeCounts[status] = 1;
        }
    }

    _taskTypeFilterAction.setOptions(_statusTypeCounts.keys());
    _taskTypeFilterAction.setSelectedOptions(_statusTypeCounts.keys());

    addTaskTypesForRows(QModelIndex(), 0, sourceModel->rowCount() - 1);

    connect(sourceModel, &QStandardItemModel::rowsInserted, this, [this](const QModelIndex& parent, int first, int last) -> void {
#ifdef TASKS_FILTER_MODEL_VERBOSE
        for (int rowIndex = first; rowIndex <= last; rowIndex++) {
            const auto sourceModelIndex = this->sourceModel()->index(rowIndex, static_cast<int>(AbstractTasksModel::Column::Name), parent);

            qDebug() << this->sourceModel() << sourceModelIndex.data(Qt::EditRole).toString() << "inserted" << sourceModelIndex << "into" << parent;
        } 
#endif
        
        invalidate();

        QTimer::singleShot(50, [this, parent, first, last]() -> void {
            addTaskTypesForRows(parent, first, last);
        });
    });

    connect(sourceModel, &QStandardItemModel::rowsAboutToBeRemoved, this, [this](const QModelIndex& parent, int first, int last) -> void {
#ifdef TASKS_FILTER_MODEL_VERBOSE
        for (int rowIndex = first; rowIndex <= last; rowIndex++) {
            const auto sourceModelIndex = this->sourceModel()->index(rowIndex, static_cast<int>(AbstractTasksModel::Column::Name), parent);

            qDebug() << this->sourceModel() << sourceModelIndex.data(Qt::EditRole).toString() << "removed" << sourceModelIndex << "from" << parent;
        }
#endif

        invalidate();

        QTimer::singleShot(50, [this, parent, first, last]() -> void {
            auto selectedTaskTypes = _taskTypeFilterAction.getSelectedOptions();

            for (int row = first; row <= last; row++) {
                const auto status = this->sourceModel()->index(row, static_cast<int>(AbstractTasksModel::Column::Type), parent).data(Qt::DisplayRole).toString();

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

    connect(this, &QSortFilterProxyModel::rowsInserted, this, [this](const QModelIndex& parent, int first, int last) -> void {
#ifdef TASKS_FILTER_MODEL_VERBOSE
        for (int rowIndex = first; rowIndex <= last; rowIndex++) {
            const auto sourceModelIndex = index(rowIndex, static_cast<int>(AbstractTasksModel::Column::Name), parent);

            qDebug() << this << sourceModelIndex.data(Qt::EditRole).toString() << "inserted" << sourceModelIndex << "into" << parent;
        }
#endif
    });

    /*
    connect(this, &QSortFilterProxyModel::rowsAboutToBeRemoved, this, [this](const QModelIndex& parent, int first, int last) -> void {
#ifdef TASKS_FILTER_MODEL_VERBOSE
        for (int rowIndex = first; rowIndex <= last; rowIndex++) {
            const auto sourceModelIndex = index(rowIndex, static_cast<int>(AbstractTasksModel::Column::Name), parent);

            qDebug() << this << sourceModelIndex.data(Qt::EditRole).toString() << "removed" << sourceModelIndex << "from" << parent;
        }
#endif
    });

    connect(this, &QSortFilterProxyModel::layoutAboutToBeChanged, this, [this]() -> void {
#ifdef TASKS_FILTER_MODEL_VERBOSE
        qDebug() << this << "layoutAboutToBeChanged";
#endif
    });

    connect(this, &QSortFilterProxyModel::layoutChanged, this, [this]() -> void {
#ifdef TASKS_FILTER_MODEL_VERBOSE
        qDebug() << this << "layoutChanged";
#endif
    });
    */

    //connect(sourceModel, &QAbstractItemModel::dataChanged, this, &TasksFilterModel::invalidate);
}

QVariant TasksFilterModel::getSourceData(const QModelIndex& index, const AbstractTasksModel::Column& column, int role) const
{
    return sourceModel()->data(index.siblingAtColumn(static_cast<int>(column)), role);
}

void TasksFilterModel::addTaskTypesForRows(const QModelIndex& parent, int first, int last)
{
    auto selectedTaskTypes = _taskTypeFilterAction.getSelectedOptions();

    for (int row = first; row <= last; row++) {
        const auto taskType = sourceModel()->index(row, static_cast<int>(AbstractTasksModel::Column::Type), parent).data(Qt::DisplayRole).toString();

        if (_statusTypeCounts.contains(taskType)) {
            _statusTypeCounts[taskType]++;
        }
        else {
            _statusTypeCounts[taskType] = 1;

            selectedTaskTypes << taskType;
        }
    }

    _taskTypeFilterAction.setOptions(_statusTypeCounts.keys());
    _taskTypeFilterAction.setSelectedOptions(selectedTaskTypes);
}

}
