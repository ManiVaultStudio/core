// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ModalTaskHandler.h"
#include "ModalTask.h"

#include "CoreInterface.h"

#ifdef _DEBUG
    //#define MODAL_TASK_HANDLER_VERBOSE
#endif

namespace hdps {

using namespace gui;

ModalTaskHandler::ModalTaskHandler(QObject* parent) :
    AbstractTaskHandler(parent, nullptr),
    _tasksAction(this, "Tasks"),
    _modalTasksDialog(this)
{
    _tasksAction.setRowHeight(30);
    _tasksAction.setProgressColumnMargin(2);

    auto& tasksModel        = _tasksAction.getTasksModel();
    auto& tasksFilterModel  = _tasksAction.getTasksFilterModel();

    tasksFilterModel.getTaskStatusFilterAction().setSelectedOptions({ "Running", "Running Indeterminate", "Finished" });
    tasksFilterModel.getTaskTypeFilterAction().selectOption("ModalTask");

    const auto updateVisibility = [this, &tasksFilterModel]() -> void {
        if (projects().hasProject() && (projects().isOpeningProject() || projects().isImportingProject()))
            return;

        const auto numberOfRows = tasksFilterModel.rowCount();

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

    _modalTasksDialog.show();
    //_modalTasksDialog.adjustSize();

    QCoreApplication::processEvents();
}

void ModalTaskHandler::destroyDialog()
{
    if (!hasDialog())
        return;

#ifdef MODAL_TASK_HANDLER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    _modalTasksDialog.close();

    QCoreApplication::processEvents();
}

bool ModalTaskHandler::hasDialog() const
{
    return _modalTasksDialog.isVisible();
}

ModalTaskHandler::ModalTasksDialog::ModalTasksDialog(ModalTaskHandler* modalTaskHandler, QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _modalTaskHandler(modalTaskHandler),
    _processEventsTimer()
{
    setWindowModality(Qt::ApplicationModal);
    
    setWindowFlag(Qt::Dialog);
    //setWindowFlag(Qt::CustomizeWindowHint);
    setWindowFlag(Qt::WindowCloseButtonHint, false);
    setWindowFlag(Qt::WindowTitleHint);
    setWindowFlag(Qt::WindowStaysOnTopHint);
    
    connect(&_processEventsTimer, &QTimer::timeout, this, [this]() -> void {
        if (isVisible())
            QCoreApplication::processEvents();
    });

    _processEventsTimer.setInterval(100);
    _processEventsTimer.start();

    auto layout = new QVBoxLayout();

    auto tasksWidget = _modalTaskHandler->getTasksAction().createWidget(this);

    tasksWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    layout->addWidget(tasksWidget);

    setLayout(layout);
    
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

        //treeView->setColumnHidden(static_cast<int>(TasksModel::Column::Name), true);
        //treeView->setColumnHidden(static_cast<int>(TasksModel::Column::Status), true);
        //treeView->setColumnHidden(static_cast<int>(TasksModel::Column::Type), true);
        
        treeView->viewport()->setBackgroundRole(QPalette::Window);
    }

    auto& tasksFilterModel = _modalTaskHandler->getTasksAction().getTasksFilterModel();


    connect(&tasksFilterModel, &QSortFilterProxyModel::rowsInserted, this, &ModalTasksDialog::numberOfModalTasksChanged);
    connect(&tasksFilterModel, &QSortFilterProxyModel::rowsRemoved, this, &ModalTasksDialog::numberOfModalTasksChanged);

    numberOfModalTasksChanged();
}

void ModalTaskHandler::ModalTasksDialog::numberOfModalTasksChanged()
{
    auto& tasksAction       = _modalTaskHandler->getTasksAction();
    auto& tasksModel        = tasksAction.getTasksModel();
    auto& tasksFilterModel  = tasksAction.getTasksFilterModel();

    const auto rowCount = tasksFilterModel.rowCount();

    if (rowCount == 0)
        return;

    /*
    auto treeView = findChild<HierarchyWidgetTreeView*>("TreeView");

    Q_ASSERT(treeView != nullptr);

    if (treeView != nullptr) {
        treeView->setColumnHidden(static_cast<int>(TasksModel::Column::Name), rowCount == 1);

        int rowsHeight = 0;

        for (int rowIndex = 0; rowIndex < rowCount; rowIndex++)
            rowsHeight += treeView->rowHeight(tasksFilterModel.index(rowIndex, 0));

        treeView->setMaximumHeight(rowsHeight);
    }
    */

    //QTimer::singleShot(100, [this]() -> void {
    //    adjustSize();
    //});

    const auto clockIcon = Application::getIconFont("FontAwesome").getIcon("clock");

    if (rowCount == 1) {
        const auto sourceModelIndex = tasksFilterModel.mapToSource(tasksFilterModel.index(0, 0));
        const auto item             = dynamic_cast<TasksModel::Item*>(tasksAction.getTasksModel().itemFromIndex(sourceModelIndex));
        const auto task             = item->getTask();
        const auto taskName         = task->getName();
        const auto taskDescription  = task->getDescription();
        const auto taskIcon         = task->getIcon();

        setWindowTitle(taskDescription.isEmpty() ? QString("Waiting for %1 to complete...").arg(taskName) : task->getDescription());
        setWindowIcon(taskIcon.isNull() ? clockIcon : taskIcon);
    }
    else {
        setWindowTitle(QString("Waiting for %1 tasks to complete...").arg(QString::number(rowCount)));
        setWindowIcon(clockIcon);
    }
}

}
