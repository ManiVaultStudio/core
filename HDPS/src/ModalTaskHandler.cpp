// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ModalTaskHandler.h"
#include "ModalTask.h"

#include "CoreInterface.h"

#ifdef _DEBUG
    #define MODAL_TASK_HANDLER_VERBOSE
#endif

namespace hdps {

using namespace gui;

ModalTaskHandler::ModalTaskHandler(QObject* parent) :
    AbstractTaskHandler(parent, nullptr),
    _tasksAction(this, "Tasks"),
    _modalTasksDialog()
{
    _tasksAction.setRowHeight(25);

    auto& tasksFilterModel = _tasksAction.getTasksFilterModel();

    tasksFilterModel.getTaskStatusFilterAction().setSelectedOptions({ "Running", "Running Indeterminate", "Finished" });
    tasksFilterModel.getTaskTypeFilterAction().selectOption("ModalTask");

    const auto updateVisibility = [this, &tasksFilterModel]() -> void {
        const auto numberOfRows = tasksFilterModel.rowCount();

        //qDebug() << "Number of modal tasks is" << numberOfRows;

        //treeView->setFixedHeight((numberOfRows + 1) * ModalTask::tasksAction.getRowHeight());

        if (numberOfRows == 0 && hasDialog())
            destroyDialog();

        if (numberOfRows >= 1 && !hasDialog())
            createDialog();
    };

    updateVisibility();

    connect(&tasksFilterModel, &QSortFilterProxyModel::rowsInserted, this, updateVisibility);
    connect(&tasksFilterModel, &QSortFilterProxyModel::rowsRemoved, this, updateVisibility);
}

void ModalTaskHandler::init()
{
}

void ModalTaskHandler::createDialog()
{
    if (hasDialog())
        return;

#ifdef MODAL_TASK_HANDLER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    _modalTasksDialog = std::make_unique<ModalTasksDialog>(this);

    _modalTasksDialog->open();

    QCoreApplication::processEvents();
}

void ModalTaskHandler::destroyDialog()
{
    if (!hasDialog())
        return;

#ifdef MODAL_TASK_HANDLER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    _modalTasksDialog->close();
    _modalTasksDialog.reset();

    QCoreApplication::processEvents();
}

bool ModalTaskHandler::hasDialog() const
{
    return _modalTasksDialog != nullptr;
}

void ModalTaskHandler::updateDialogVisibility()
{

}

ModalTaskHandler::ModalTasksDialog::ModalTasksDialog(ModalTaskHandler* modalTaskHandler, QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _modalTaskHandler(modalTaskHandler)
{
    auto layout = new QVBoxLayout();

    layout->addWidget(_modalTaskHandler->getTasksAction().createWidget(this));

    setLayout(layout);

    auto hierarchyWidget = findChild<HierarchyWidget*>("HierarchyWidget");

    Q_ASSERT(hierarchyWidget != nullptr);

    if (hierarchyWidget != nullptr)
        hierarchyWidget->setHeaderHidden(true);

    auto treeView = findChild<QTreeView*>("TreeView");

    Q_ASSERT(treeView != nullptr);

    if (treeView != nullptr) {
        treeView->setStyleSheet("QTreeView { border: none; }");

        treeView->setColumnHidden(static_cast<int>(TasksModel::Column::Name), true);
        treeView->setColumnHidden(static_cast<int>(TasksModel::Column::Status), true);
        treeView->setColumnHidden(static_cast<int>(TasksModel::Column::Type), true);

        QPalette palette(treeView->palette());

        palette.setColor(QPalette::Base, QWidget::palette().color(QWidget::backgroundRole()));

        treeView->setPalette(palette);
    }

    auto& tasksFilterModel = _modalTaskHandler->getTasksAction().getTasksFilterModel();

    connect(&tasksFilterModel, &QSortFilterProxyModel::rowsInserted, this, &ModalTasksDialog::updateWindowTitleAndIcon);
    connect(&tasksFilterModel, &QSortFilterProxyModel::rowsRemoved, this, &ModalTasksDialog::updateWindowTitleAndIcon);

    updateWindowTitleAndIcon();
}

void ModalTaskHandler::ModalTasksDialog::updateWindowTitleAndIcon()
{
    auto& tasksAction       = _modalTaskHandler->getTasksAction();
    auto& taskFilterModel   = tasksAction.getTasksFilterModel();

    const auto rowCount = taskFilterModel.rowCount();

    if (rowCount == 1) {
        const auto sourceModelIndex = taskFilterModel.mapToSource(taskFilterModel.index(0, 0));
        const auto item             = dynamic_cast<TasksModel::Item*>(tasksAction.getTasksModel().itemFromIndex(sourceModelIndex));
        const auto task             = item->getTask();

        setWindowTitle(task->getDescription());
        setWindowIcon(task->getIcon());
    }
    else {
        setWindowTitle(QString("Waiting for %1 tasks to complete...").arg(QString::number(rowCount)));
        setWindowIcon(Application::getIconFont("FontAwesome").getIcon("hourglass-half"));
    }
}

}
