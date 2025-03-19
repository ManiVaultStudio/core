// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ForegroundTasksStatusBarAction.h"

#ifdef _DEBUG
    #define FOREGROUND_TASKS_STATUS_BAR_ACTION_VERBOSE
#endif

#include <QApplication> 
#include <QDebug>

using namespace mv;
using namespace mv::gui;

ForegroundTasksStatusBarAction::ForegroundTasksStatusBarAction(QObject* parent, const QString& title) :
    StatusBarAction(parent, title, "list-check"),
    _tasksAction(this, "Tasks"),
    _numberOfTasks(0)
{
    setToolTip("Foreground tasks");
    setEnabled(false);
    setPopupAction(&_tasksAction);

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
    _tasksAction.setWidgetConfigurationFunction([this](WidgetAction* action, QWidget* widget) -> void {
        Q_ASSERT(widget);

        auto toolButtonMenu = dynamic_cast<WidgetActionToolButtonMenu*>(widget->parentWidget()->parentWidget());

        //toolButtonMenu->setIgnoreCloseEvent(true);
    });

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
            showPopup();
        }

        if (_numberOfTasks >= 1 && numberOfTasks == 0) {
            setEnabled(false);
            hidePopup();
        }

        _numberOfTasks = numberOfTasks;
    };

    numberOfTasksChanged();

    connect(&_filterModel, &QSortFilterProxyModel::rowsInserted, this, numberOfTasksChanged, Qt::DirectConnection);
    connect(&_filterModel, &QSortFilterProxyModel::rowsRemoved, this, numberOfTasksChanged, Qt::DirectConnection);
}
