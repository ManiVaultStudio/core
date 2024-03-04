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

    _filterModel.setSourceModel(&_model);
    _filterModel.getTaskScopeFilterAction().setSelectedOptions({ "Foreground" });
    _filterModel.getTaskStatusFilterAction().setSelectedOptions({ "Running Indeterminate", "Running", "Finished", "Aborting" });
    _filterModel.getTopLevelTasksOnlyAction().setChecked(true);

    _tasksAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::NoGroupBoxInPopupLayout);
    _tasksAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    _tasksAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ToolButtonAutoRaise);
    _tasksAction.setPopupSizeHint(QSize(600, 0));
    _tasksAction.setIcon(QIcon());
    _tasksAction.initialize(&_model, &_filterModel, "Foreground Task");

    auto& badge = getBarIconStringAction().getBadge();

    badge.setScale(0.5f);
    badge.setBackgroundColor(qApp->palette().highlight().color());

    const auto numberOfTasksChanged = [this, &badge]() -> void {
        const auto numberOfTasks = _filterModel.rowCount();

        if (numberOfTasks == _numberOfTasks)
            return;

        badge.setEnabled(numberOfTasks > 0);
        badge.setNumber(numberOfTasks);

        if (_numberOfTasks == 0 && numberOfTasks >= 1) {
            setEnabled(true);
            setPopupAction(&_tasksAction);
            showPopup();
        }

        if (_numberOfTasks >= 1 && numberOfTasks == 0) {
            setEnabled(false);
            setPopupAction(nullptr);
            hidePopup();
        }

        _numberOfTasks = numberOfTasks;
    };

    numberOfTasksChanged();

    _numberOfTasksTimer.setInterval(100);
    _numberOfTasksTimer.callOnTimeout(this, numberOfTasksChanged);
    _numberOfTasksTimer.start();
}
