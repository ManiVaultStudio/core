// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ForegroundTasksStatusBarAction.h"

#ifdef _DEBUG
    #define FOREGROUND_TASKS_STATUS_BAR_ACTION_VERBOSE
#endif

#include <QDebug>

using namespace mv;
using namespace mv::gui;

ForegroundTasksStatusBarAction::ForegroundTasksStatusBarAction(QObject* parent, const QString& title) :
    StatusBarAction(parent, title, "tasks"),
    _model(),
    _filterModel(),
    _tasksAction(this, "Tasks"),
    _numberOfTasks(0),
    _numberOfTasksTimer()
{
    setToolTip("Foreground tasks");
    setEnabled(false);

    _filterModel.getTaskScopeFilterAction().setSelectedOptions({ "Foreground" });
    _filterModel.getTaskStatusFilterAction().setSelectedOptions({ "Running Indeterminate", "Running", "Finished", "Aborting" });
    _filterModel.getTopLevelTasksOnlyAction().setChecked(true);

    _tasksAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::NoGroupBoxInPopupLayout);
    _tasksAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    _tasksAction.setPopupSizeHint(QSize(600, 150));
    _tasksAction.initialize(&_model, &_filterModel, "Foreground Task");
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
        treeView.setColumnHidden(static_cast<int>(AbstractTasksModel::Column::ParentID), true);
        treeView.setColumnHidden(static_cast<int>(AbstractTasksModel::Column::Type), true);

        treeView.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        treeView.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        const auto numberOfForegroundTasksChanged = [this, &treeView, hierarchyWidget, widget]() -> void {
            const auto numberOfTasks = _filterModel.rowCount();

            std::int32_t height = 0;

            for (int rowIndex = 0; rowIndex < _filterModel.rowCount(); ++rowIndex)
                height += treeView.sizeHintForRow(rowIndex);

            hierarchyWidget->setFixedHeight(height);

            treeView.setColumnHidden(static_cast<int>(AbstractTasksModel::Column::Kill), !_filterModel.hasKillableTasks());
        };

        numberOfForegroundTasksChanged();

        connect(&_filterModel, &QSortFilterProxyModel::rowsInserted, &treeView, numberOfForegroundTasksChanged);
        connect(&_filterModel, &QSortFilterProxyModel::rowsRemoved, &treeView, numberOfForegroundTasksChanged);
    });

    auto& badge = getBarIconStringAction().getBadge();

    badge.setScale(0.5f);
    badge.setBackgroundColor(qApp->palette().highlight().color());

    const auto numberOfBackgroundTasksChanged = [this, &badge]() -> void {
        const auto numberOfTasks = _filterModel.rowCount();

        if (numberOfTasks == _numberOfTasks)
            return;

        badge.setEnabled(numberOfTasks > 0);
        badge.setNumber(numberOfTasks);

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
