// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "BackgroundTasksStatusBarAction.h"
#include "BackgroundTask.h"
#include "BackgroundTaskHandler.h"

#ifdef _DEBUG
    #define BACKGROUND_TASKS_STATUS_BAR_ACTION_VERBOSE
#endif

#include <QDebug>

namespace mv::gui {

BackgroundTasksStatusBarAction::BackgroundTasksStatusBarAction(QObject* parent, const QString& title) :
    StatusBarAction(parent, title),
    _barGroupAction(this, "Bar group"),
    _overallBackgroundTaskAction(this, "Overall background task"),
    _model(),
    _filterModel(),
    _tasksAction(this, "Background tasks")
{
    setBarAction(&_barGroupAction);

    _barGroupAction.setShowLabels(false);
    _barGroupAction.addAction(&_overallBackgroundTaskAction);

    auto& overallBackgroundTask = BackgroundTask::getGlobalHandler()->getOverallBackgroundTask();

    _overallBackgroundTaskAction.setStretch(1);
    _overallBackgroundTaskAction.setTask(&overallBackgroundTask);

    const auto overallBackgroundTaskTextFormatter = [this](Task& task) -> QString {
        const auto numberOfChildTasks = task.getChildTasksForGuiScopesAndStatuses(false, true, { Task::GuiScope::Background }, { Task::Status::Running, Task::Status::RunningIndeterminate }).count();

        switch (task.getStatus())
        {
            case Task::Status::Undefined:
            case Task::Status::Idle:
                return "No background tasks";

            case Task::Status::Running:
            case Task::Status::RunningIndeterminate:
                return QString("%1 background task%2 %3 %4%").arg(QString::number(numberOfChildTasks), numberOfChildTasks == 1 ? "" : "s", numberOfChildTasks == 1 ? "is" : "are", QString::number(task.getProgress() * 100.f, 'f', 1));

            case Task::Status::Finished:
                return QString("All background tasks have finished");

            case Task::Status::AboutToBeAborted:
            case Task::Status::Aborting:
            case Task::Status::Aborted:
                return {};

            default:
                break;
        }

        return {};
    };

    overallBackgroundTask.setProgressTextFormatter(overallBackgroundTaskTextFormatter);

    _overallBackgroundTaskAction.setIcon(Application::getIconFont("FontAwesome").getIcon("search"));

    _filterModel.setSourceModel(&_model);

    _filterModel.getTaskScopeFilterAction().setSelectedOptions({ "Background" });
    _filterModel.getTaskStatusFilterAction().setSelectedOptions({ "Running Indeterminate", "Running", "Aborting" });
    _filterModel.getParentTaskFilterAction().setString(overallBackgroundTask.getId());
    _filterModel.setFilterKeyColumn(static_cast<int>(AbstractTasksModel::Column::Name));

    _tasksAction.initialize(&_model, &_filterModel, "Background task");
    _tasksAction.setPopupSizeHint(QSize(600, 0));

    auto& badge = getBadge();

    badge.setScale(0.5f);
    badge.setBackgroundColor(qApp->palette().highlight().color());

    const auto numberOfBackgroundTasksChanged = [this, &badge]() -> void {
        const auto numberOfRecords = _filterModel.rowCount();

        badge.setEnabled(numberOfRecords > 0);
        badge.setNumber(numberOfRecords);

        setPopupAction(numberOfRecords > 0 ? &_tasksAction : nullptr);
    };

    numberOfBackgroundTasksChanged();

    connect(&_filterModel, &QSortFilterProxyModel::rowsInserted, this, numberOfBackgroundTasksChanged);
    connect(&_filterModel, &QSortFilterProxyModel::rowsRemoved, this, numberOfBackgroundTasksChanged);
    connect(&_filterModel, &QSortFilterProxyModel::layoutChanged, this, numberOfBackgroundTasksChanged);
}

}
