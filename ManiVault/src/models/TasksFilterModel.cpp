// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TasksFilterModel.h"

#include <QDebug>

#ifdef _DEBUG
    //#define TASKS_FILTER_MODEL_VERBOSE
#endif

using namespace mv::gui;

namespace mv
{

TasksFilterModel::TasksFilterModel(QObject* parent /*= nullptr*/) :
    QSortFilterProxyModel(parent),
    _taskTypeFilterAction(this, "Type"),
    _taskGuiScopeFilterAction(this, "GUI Scope", Task::guiScopeNames.values(), Task::guiScopeNames.values()),
    _taskStatusFilterAction(this, "Status", Task::statusNames.values(), Task::statusNames.values()),
    _topLevelTasksOnlyAction(this, "Top-level tasks only", false),
    _hideDisabledTasksFilterAction(this, "Hide disabled tasks", true),
    _hideHiddenTasksFilterAction(this, "Hide hidden tasks", true),
    _parentTaskFilterAction(this, "Parent task identifier"),
    _statusTypeCounts(),
    _rowCount(0)
{
    setDynamicSortFilter(true);
    setRecursiveFilteringEnabled(true);

    const auto taskTypes = QStringList({ "Task", "BackgroundTask", "DatasetTask", "ForegroundTask", "ModalTask", "ProjectSerializationTask", "ApplicationStartupTask"});

    _taskTypeFilterAction.initialize(taskTypes, taskTypes);

    _taskTypeFilterAction.setDefaultWidgetFlags(OptionsAction::ComboBox | OptionsAction::Selection);
    _taskGuiScopeFilterAction.setDefaultWidgetFlags(OptionsAction::ComboBox | OptionsAction::Selection);
    _taskStatusFilterAction.setDefaultWidgetFlags(OptionsAction::ComboBox | OptionsAction::Selection);

    connect(&_taskTypeFilterAction, &OptionsAction::selectedOptionsChanged, this, &TasksFilterModel::invalidate);
    connect(&_taskGuiScopeFilterAction, &OptionsAction::selectedOptionsChanged, this, &TasksFilterModel::invalidate);
    connect(&_taskStatusFilterAction, &OptionsAction::selectedOptionsChanged, this, &TasksFilterModel::invalidate);
    connect(&_topLevelTasksOnlyAction, &ToggleAction::toggled, this, &TasksFilterModel::invalidate);
    connect(&_hideDisabledTasksFilterAction, &ToggleAction::toggled, this, &TasksFilterModel::invalidate);
    connect(&_hideHiddenTasksFilterAction, &ToggleAction::toggled, this, &TasksFilterModel::invalidate);
    connect(&_parentTaskFilterAction, &StringAction::stringChanged, this, &TasksFilterModel::invalidate);

    _taskTypeFilterAction.setToolTip("Filter tasks based on their type");
    _taskGuiScopeFilterAction.setToolTip("Filter tasks based on their GUI scope");
    _taskStatusFilterAction.setToolTip("Filter tasks based on their status");
    _topLevelTasksOnlyAction.setToolTip("When checked shows top-level tasks only");
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

    if (_taskTypeFilterAction.hasSelectedOptions() && !taskTypes.contains(taskType))
        return false;

    const auto taskGuiScopesSelection   = _taskGuiScopeFilterAction.getSelectedOptions();
    const auto taskGuiScopes            = getSourceData(index, AbstractTasksModel::Column::GuiScopes, Qt::EditRole).toStringList();

    auto taskGuiScopesSelectionSet  = QSet(taskGuiScopesSelection.begin(), taskGuiScopesSelection.end());
    auto taskGuiScopesSet           = QSet(taskGuiScopes.begin(), taskGuiScopes.end());

    if (_taskGuiScopeFilterAction.hasSelectedOptions() && taskGuiScopesSelectionSet.intersect(taskGuiScopesSet).isEmpty())
        return false;

    const auto taskStatusTypes  = _taskStatusFilterAction.getSelectedOptions();
    const auto statusIndex      = getSourceData(index, AbstractTasksModel::Column::Status, Qt::EditRole).toInt();
    const auto taskStatus       = Task::statusNames[static_cast<Task::Status>(statusIndex)];
    
    if (!taskStatusTypes.contains(taskStatus))
        return false;

    if (_topLevelTasksOnlyAction.isChecked())
        if (parent != QModelIndex())
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

bool TasksFilterModel::hasKillableTasks() const
{
    for (int rowIndex = 0; rowIndex < rowCount(); ++rowIndex)
        if (index(rowIndex, static_cast<int>(AbstractTasksModel::Column::MayKill), QModelIndex()).data(Qt::EditRole).toBool())
            return true;

    return false;
}

}
