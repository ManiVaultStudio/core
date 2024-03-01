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
    StatusBarAction(parent, title, "globe"),
    _model(),
    _filterModel(),
    _overallBackgroundTaskAction(this, "Overall background task"),
    _tasksAction(this, "Background tasks"),
    _numberOfTasks(0),
    _numberOfTasksTimer()
{
    setToolTip("Background tasks");
    setEnabled(false);

    auto& overallBackgroundTask = BackgroundTask::getGlobalHandler()->getOverallBackgroundTask();

    _filterModel.setSourceModel(&_model);
    _filterModel.getTaskScopeFilterAction().setSelectedOptions({ "Background" });
    _filterModel.getTaskStatusFilterAction().setSelectedOptions({ "Running Indeterminate", "Running", "Aborting" });
    _filterModel.getParentTaskFilterAction().setString(overallBackgroundTask.getId());
    _filterModel.setFilterKeyColumn(static_cast<int>(AbstractTasksModel::Column::Name));

    getBarGroupAction().addAction(&_overallBackgroundTaskAction);

    _overallBackgroundTaskAction.setEnabled(false);
    _overallBackgroundTaskAction.setStretch(1);
    _overallBackgroundTaskAction.setTask(&overallBackgroundTask);
    _overallBackgroundTaskAction.setToolTip(toolTip());
    _overallBackgroundTaskAction.getProgressAction().setDefaultWidgetFlags(ProgressAction::Label);

    const auto overallBackgroundTaskTextFormatter = [this](Task& task) -> QString {
        const auto numberOfChildTasks = task.getChildTasksForGuiScopesAndStatuses(false, true, { Task::GuiScope::Background }, { Task::Status::Running, Task::Status::RunningIndeterminate }).count();

        switch (task.getStatus())
        {
            case Task::Status::Undefined:
            case Task::Status::Idle:
                return "No background tasks";

            case Task::Status::Running:
            case Task::Status::RunningIndeterminate:
                return QString("%1 background task%2: %3%").arg(QString::number(numberOfChildTasks), numberOfChildTasks == 1 ? "" : "s", QString::number(task.getProgress() * 100.f, 'f', 1));

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
    _tasksAction.setIcon(QIcon());
    _tasksAction.initialize(&_model, &_filterModel, "Background task");
    _tasksAction.setWidgetConfigurationFunction([this](WidgetAction* action, QWidget* widget) -> void {
        auto hierarchyWidget = widget->findChild<HierarchyWidget*>("HierarchyWidget");

        Q_ASSERT(hierarchyWidget);

        if (hierarchyWidget == nullptr)
            return;

        hierarchyWidget->getToolbarAction().setVisible(false);
        hierarchyWidget->setHeaderHidden(true);

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

        treeView.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        treeView.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

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
        const auto numberOfTasks = _filterModel.rowCount();

        if (numberOfTasks == _numberOfTasks)
            return;

        if (_numberOfTasks == 0 && numberOfTasks >= 1) {
            setEnabled(true);
            setPopupAction(&_tasksAction);
        }

        if (_numberOfTasks >= 1 && numberOfTasks == 0) {
            setEnabled(false);
            setPopupAction(nullptr);
        }

        _numberOfTasks = numberOfTasks;
    };

    numberOfBackgroundTasksChanged();

    _numberOfTasksTimer.setInterval(100);
    _numberOfTasksTimer.callOnTimeout(this, numberOfBackgroundTasksChanged);
    _numberOfTasksTimer.start();
}
