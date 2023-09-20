// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ForegroundTaskHandler.h"
#include "Application.h"
#include "CoreInterface.h"

using namespace hdps::gui;

namespace hdps {

ForegroundTaskHandler::ForegroundTaskHandler(QObject* parent) :
    AbstractTaskHandler(parent, nullptr),
    _foregroundTasksAction(this, "Foreground Tasks")
{
    
}

void ForegroundTaskHandler::init()
{
}

ForegroundTaskHandler::ForegroundTasksAction::ForegroundTasksAction(QObject* parent, const QString& title) :
    WidgetAction(parent, title),
    _tasksAction(this, "Foreground Tasks")
{
    setIcon(Application::getIconFont("FontAwesome").getIcon("list"));

    _tasksAction.setRowHeight(26);
    _tasksAction.setProgressColumnMargin(2);

    auto& tasksModel        = _tasksAction.getTasksModel();
    auto& tasksFilterModel  = _tasksAction.getTasksFilterModel();

    tasksFilterModel.getTaskStatusFilterAction().setSelectedOptions({ "Running", "Running Indeterminate", "Finished" });
    tasksFilterModel.getTaskTypeFilterAction().setSelectedOptions({ "ForegroundTask" });
}

ForegroundTaskHandler::ForegroundTasksAction::Widget::Widget(QWidget* parent, ForegroundTasksAction* foregroundTasksAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, foregroundTasksAction, widgetFlags)
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    auto& tasksAction = foregroundTasksAction->getTasksAction();

    auto tasksWidget = tasksAction.createWidget(this);

    //layout->addWidget(tasksWidget);

    auto hierarchyWidget = findChild<HierarchyWidget*>("HierarchyWidget");

    Q_ASSERT(hierarchyWidget != nullptr);

    if (hierarchyWidget != nullptr)
        hierarchyWidget->setHeaderHidden(true);

    auto treeView = findChild<QTreeView*>("TreeView");

    Q_ASSERT(treeView != nullptr);

    if (treeView != nullptr) {
        treeView->setStyleSheet("QTreeView { border: none; }");

        treeView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        treeView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        treeView->setColumnHidden(static_cast<int>(TasksModel::Column::Name), true);
        treeView->setColumnHidden(static_cast<int>(TasksModel::Column::Status), true);
        treeView->setColumnHidden(static_cast<int>(TasksModel::Column::Type), true);

        treeView->viewport()->setBackgroundRole(QPalette::Window);
    }

    setLayout(layout);

    auto& tasksModel        = tasksAction.getTasksModel();
    auto& tasksFilterModel  = tasksAction.getTasksFilterModel();

    const auto updateVisibility = [this, &tasksFilterModel, tasksWidget]() -> void {
        if (projects().hasProject() && (projects().isOpeningProject() || projects().isImportingProject()))
            return;

        const auto numberOfRows = tasksFilterModel.rowCount();

        tasksWidget->setVisible(numberOfRows >= 1);

        qDebug() << "Number of foreground tasks: " << numberOfRows;
    };

    updateVisibility();

    connect(&tasksFilterModel, &QSortFilterProxyModel::rowsInserted, this, updateVisibility);
    connect(&tasksFilterModel, &QSortFilterProxyModel::rowsRemoved, this, updateVisibility);
}

}
