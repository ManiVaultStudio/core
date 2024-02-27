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
    StatusBarAction(parent, title),
    _model(),
    _filterModel(),
    _barGroupAction(this, "Bar group"),
    _tasksAction(this, "Tasks")
{
    setBarAction(&_barGroupAction);
    setPopupAction(&_tasksAction);

    _barGroupAction.setShowLabels(false);
    //_barGroupAction.addAction(&_tasksAction);

    _filterModel.getTaskScopeFilterAction().setSelectedOptions({ "Foreground" });
    _filterModel.getTaskStatusFilterAction().setSelectedOptions({ "Running Indeterminate", "Running", "Finished", "Aborting" });
    _filterModel.getTopLevelTasksOnlyAction().setChecked(true);

    _tasksAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::NoGroupBoxInPopupLayout);
    _tasksAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    //_tasksAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ToolButtonAutoRaise);
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
        treeView.setColumnHidden(static_cast<int>(AbstractTasksModel::Column::Type), true);
    });

    auto& badge = _tasksAction.getBadge();

    badge.setScale(0.5f);
    badge.setBackgroundColor(qApp->palette().highlight().color());

    const auto updateBadgeNumber = [this, &badge]() -> void {
        const auto numberOfRecords = _filterModel.rowCount();

        badge.setEnabled(numberOfRecords > 0);
        badge.setNumber(numberOfRecords);
    };

    updateBadgeNumber();

    connect(&_filterModel, &QSortFilterProxyModel::rowsInserted, this, updateBadgeNumber);
    connect(&_filterModel, &QSortFilterProxyModel::rowsRemoved, this, updateBadgeNumber);
    connect(&_filterModel, &QSortFilterProxyModel::layoutChanged, this, updateBadgeNumber);

    /*
    _tasksStatusBarAction.setPopupMode(TasksStatusBarAction::PopupMode::Automatic);
    _tasksStatusBarAction.getMenu().addAction(&settings().getTasksSettingsAction().getHideForegroundTasksPopupAction());

    _statusBarAction.addAction(&_tasksStatusBarAction);
    _statusBarAction.setShowLabels(false);

    const auto hideForegroundTasksPopupChanged = [this]() -> void {
        const auto hideForegroundTasksPopup = settings().getTasksSettingsAction().getHideForegroundTasksPopupAction().isChecked();

        _tasksStatusBarAction.setPopupMode(hideForegroundTasksPopup ? TasksStatusBarAction::PopupMode::Hover : TasksStatusBarAction::PopupMode::Automatic);
        _tasksStatusBarAction.setPopupForceHidden(hideForegroundTasksPopup);
    };

    hideForegroundTasksPopupChanged();

    connect(&settings().getTasksSettingsAction().getHideForegroundTasksPopupAction(), &ToggleAction::toggled, this, hideForegroundTasksPopupChanged);
    */
}
