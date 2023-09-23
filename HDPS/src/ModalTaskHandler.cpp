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

    _deferShowTimer.setSingleShot(true);
    _deferShowTimer.setInterval(250);

    auto& tasksModel        = _tasksAction.getTasksModel();
    auto& tasksFilterModel  = _tasksAction.getTasksFilterModel();

    tasksFilterModel.getTaskStatusFilterAction().setSelectedOptions({ "Running", "Running Indeterminate", "Finished" });
    tasksFilterModel.getTaskScopeFilterAction().setSelectedOptions({ "Modal" });

    const auto updateVisibility = [this, &tasksFilterModel]() -> void {
        const auto numberOfRows = tasksFilterModel.rowCount();

        if (numberOfRows == 0 && _modalTasksDialog.isVisible())
            _modalTasksDialog.close();

        if (numberOfRows >= 1 && !_modalTasksDialog.isVisible()) {
            if (!_deferShowTimer.isActive()) {
                _deferShowTimer.start();
            }
            else {
                _deferShowTimer.stop();
                _deferShowTimer.start();
            }
        }

        QCoreApplication::processEvents();
    };

    updateVisibility();

    connect(&_deferShowTimer, &QTimer::timeout, this, &ModalTaskHandler::showDialog);
    connect(&tasksFilterModel, &QSortFilterProxyModel::layoutChanged, this, updateVisibility);
    connect(&tasksFilterModel, &QSortFilterProxyModel::rowsRemoved, this, updateVisibility);
}

void ModalTaskHandler::init()
{
}

void ModalTaskHandler::showDialog()
{
    if (_tasksAction.getTasksFilterModel().rowCount() >= 1)
        _modalTasksDialog.show();
}

ModalTaskHandler::ModalTasksDialog::ModalTasksDialog(ModalTaskHandler* modalTaskHandler, QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _modalTaskHandler(modalTaskHandler)
{
    setWindowModality(Qt::ApplicationModal);
    
    setWindowFlag(Qt::Dialog);
    setWindowFlag(Qt::WindowCloseButtonHint, false);
    setWindowFlag(Qt::WindowTitleHint);
    setWindowFlag(Qt::WindowStaysOnTopHint);
    
    _deferPopulateTimer.setSingleShot(true);
    _deferPopulateTimer.setInterval(250);

    setLayout(new QVBoxLayout());
    
    auto& tasksFilterModel = _modalTaskHandler->getTasksAction().getTasksFilterModel();

    const auto numberOfModalTasksChangedDeferred = [this]() -> void {
        if (!_deferPopulateTimer.isActive()) {
            _deferPopulateTimer.start();
        } else {
            _deferPopulateTimer.stop();
            _deferPopulateTimer.start();
        }
    };

    connect(&_deferPopulateTimer, &QTimer::timeout, this, &ModalTasksDialog::numberOfModalTasksChanged);
    connect(&tasksFilterModel, &QSortFilterProxyModel::layoutChanged, this, numberOfModalTasksChangedDeferred);
    connect(&tasksFilterModel, &QSortFilterProxyModel::rowsRemoved, this, numberOfModalTasksChangedDeferred);

    numberOfModalTasksChanged();
}

void ModalTaskHandler::ModalTasksDialog::numberOfModalTasksChanged()
{
    auto& tasksAction       = _modalTaskHandler->getTasksAction();
    auto& tasksModel        = tasksAction.getTasksModel();
    auto& tasksFilterModel  = tasksAction.getTasksFilterModel();

    const auto numberOfModalTasks = tasksFilterModel.rowCount();

    if (numberOfModalTasks == 0)
        return;

    cleanLayout();

    QVector<Task*> currentTasks;

    for (int rowIndex = 0; rowIndex < numberOfModalTasks; ++rowIndex) {
        const auto sourceModelIndex = tasksFilterModel.mapToSource(tasksFilterModel.index(rowIndex, static_cast<int>(TasksModel::Column::Progress)));

        if (!sourceModelIndex.isValid())
            continue;

        auto progressItem = dynamic_cast<TasksModel::ProgressItem*>(tasksModel.itemFromIndex(sourceModelIndex));

        Q_ASSERT(progressItem != nullptr);

        if (progressItem == nullptr)
            continue;

        currentTasks << progressItem->getTask();

        QWidget* taskWidget = nullptr;

        if (!_widgetsMap.contains(progressItem->getTask())) {
            _widgetsMap[progressItem->getTask()] = progressItem->getTaskAction().createWidget(this);
        }
        else {
            taskWidget = _widgetsMap[progressItem->getTask()];
        }

        layout()->addWidget(_widgetsMap[progressItem->getTask()]);
    }

    for (auto task : _widgetsMap.keys()) {
        if (currentTasks.contains(task))
            continue;

        delete _widgetsMap[task];
        _widgetsMap.remove(task);
    }

    adjustSize();

    const auto clockIcon = Application::getIconFont("FontAwesome").getIcon("clock");

    if (numberOfModalTasks == 1) {
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
        setWindowTitle(QString("Waiting for %1 tasks to complete...").arg(QString::number(numberOfModalTasks)));
        setWindowIcon(clockIcon);
    }
}

void ModalTaskHandler::ModalTasksDialog::cleanLayout()
{
    QLayoutItem* item;

    while ((item = this->layout()->takeAt(0)) != 0)
        delete item;
}

}
