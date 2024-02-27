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

using namespace mv;
using namespace mv::gui;

BackgroundTasksStatusBarAction::BackgroundTasksStatusBarAction(QObject* parent, const QString& title) :
    StatusBarAction(parent, title),
    _model(),
    _filterModel(),
    _overallBackgroundTaskAction(this, "Overall background task"),
    _tasksAction(this, "Background tasks")
{
    auto& overallBackgroundTask = BackgroundTask::getGlobalHandler()->getOverallBackgroundTask();

    _filterModel.setSourceModel(&_model);
    _filterModel.getTaskScopeFilterAction().setSelectedOptions({ "Background" });
    _filterModel.getTaskStatusFilterAction().setSelectedOptions({ "Running Indeterminate", "Running", "Aborting" });
    _filterModel.getParentTaskFilterAction().setString(overallBackgroundTask.getId());
    _filterModel.setFilterKeyColumn(static_cast<int>(AbstractTasksModel::Column::Name));

    getBarGroupAction().addAction(&_overallBackgroundTaskAction);

    _overallBackgroundTaskAction.setStretch(1);
    _overallBackgroundTaskAction.setVisible(false);
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
                return QString("%1 background task%2 %3%").arg(QString::number(numberOfChildTasks), numberOfChildTasks == 1 ? "" : "s", QString::number(task.getProgress() * 100.f, 'f', 1));

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

    _tasksAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    _tasksAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::NoGroupBoxInPopupLayout);
    _tasksAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ToolButtonAutoRaise);
    _tasksAction.setPopupSizeHint(QSize(600, 0));
    _tasksAction.initialize(&_model, &_filterModel, "Background task");
    _tasksAction.setWidgetConfigurationFunction([this](WidgetAction* action, QWidget* widget) -> void {
        widget->setMinimumHeight(5);
        widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);

        auto hierarchyWidget = widget->findChild<HierarchyWidget*>("HierarchyWidget");

        Q_ASSERT(hierarchyWidget);

        if (hierarchyWidget == nullptr)
            return;

        hierarchyWidget->getToolbarAction().setVisible(false);
        hierarchyWidget->setHeaderHidden(true);
        hierarchyWidget->setStyleSheet("background-color: red;");

        auto& treeView = hierarchyWidget->getTreeView();

        auto palette = treeView.palette();

        palette.setColor(QPalette::Base, QApplication::palette().color(QPalette::Normal, QPalette::Window));

        treeView.setAutoFillBackground(true);
        treeView.setFrameShape(QFrame::NoFrame);
        treeView.setPalette(palette);
        treeView.viewport()->setPalette(palette);
        treeView.setRootIsDecorated(false);

        treeView.setColumnHidden(static_cast<int>(AbstractTasksModel::Column::Status), true);
        treeView.setColumnHidden(static_cast<int>(AbstractTasksModel::Column::Type), true);

        const auto numberOfBackgroundTasksChanged = [this, &treeView, hierarchyWidget, widget]() -> void {
            std::int32_t height = 0;

            for (int rowIndex = 0; rowIndex < _filterModel.rowCount(); ++rowIndex)
                height += treeView.sizeHintForRow(rowIndex);

            hierarchyWidget->setFixedHeight(height);

            treeView.setColumnHidden(static_cast<int>(AbstractTasksModel::Column::Kill), !_filterModel.hasKillableTasks());
        };

        numberOfBackgroundTasksChanged();

        connect(&_filterModel, &QSortFilterProxyModel::rowsInserted, &treeView, numberOfBackgroundTasksChanged);
        connect(&_filterModel, &QSortFilterProxyModel::rowsRemoved, &treeView, numberOfBackgroundTasksChanged);
    });

    const auto numberOfBackgroundTasksChanged = [this]() -> void {
        setPopupAction(_filterModel.rowCount() > 0 ? &_tasksAction : nullptr);
    };

    numberOfBackgroundTasksChanged();

    connect(&_filterModel, &QSortFilterProxyModel::rowsInserted, this, numberOfBackgroundTasksChanged);
    connect(&_filterModel, &QSortFilterProxyModel::rowsRemoved, this, numberOfBackgroundTasksChanged);
    connect(&_filterModel, &QSortFilterProxyModel::layoutChanged, this, numberOfBackgroundTasksChanged);
}
