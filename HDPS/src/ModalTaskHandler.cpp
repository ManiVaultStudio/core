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

    _minimumDurationTimer.setSingleShot(true);
    
    const auto updateMinimumDurationTimer = [this]() -> void {
        _minimumDurationTimer.setInterval(getMinimumDuration());
    };

    updateMinimumDurationTimer();

    connect(this, &AbstractTaskHandler::minimumDurationChanged, this, updateMinimumDurationTimer);

    auto& tasksModel        = _tasksAction.getTasksModel();
    auto& tasksFilterModel  = _tasksAction.getTasksFilterModel();

    tasksFilterModel.getTaskStatusFilterAction().setSelectedOptions({ "Running", "Running Indeterminate", "Finished" });
    tasksFilterModel.getTaskScopeFilterAction().setSelectedOptions({ "Modal" });

    const auto updateVisibilityDeferred = [this]() -> void {
        if (_modalTasksDialog.isHidden() && !_minimumDurationTimer.isActive())
            _minimumDurationTimer.start();

        if (_modalTasksDialog.isVisible())
            updateDialogVisibility();
    };

    connect(&_minimumDurationTimer, &QTimer::timeout, this, &ModalTaskHandler::updateDialogVisibility);

    connect(&tasksFilterModel, &QSortFilterProxyModel::layoutChanged, this, updateVisibilityDeferred);
    connect(&tasksFilterModel, &QSortFilterProxyModel::rowsRemoved, this, updateVisibilityDeferred);
}

void ModalTaskHandler::init()
{
}

void ModalTaskHandler::updateDialogVisibility()
{
    const auto numberOfModalTasks = _tasksAction.getTasksFilterModel().rowCount();

    if (numberOfModalTasks == 0 && _modalTasksDialog.isVisible())
        _modalTasksDialog.close();

    if (numberOfModalTasks >= 1 && !_modalTasksDialog.isVisible())
        _modalTasksDialog.show();
}

ModalTaskHandler::ModalTasksDialog::ModalTasksDialog(ModalTaskHandler* modalTaskHandler, QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _modalTaskHandler(modalTaskHandler)
{
    setWindowModality(Qt::ApplicationModal);

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    setFixedWidth(1000);

    setWindowFlag(Qt::Dialog);
    setWindowFlag(Qt::WindowCloseButtonHint, false);
    setWindowFlag(Qt::WindowTitleHint);
    setWindowFlag(Qt::WindowStaysOnTopHint);
    
    setLayout(new QVBoxLayout());
    
    auto& tasksFilterModel = _modalTaskHandler->getTasksAction().getTasksFilterModel();

    connect(&tasksFilterModel, &QSortFilterProxyModel::layoutChanged, this, &ModalTasksDialog::numberOfModalTasksChanged);
    connect(&tasksFilterModel, &QSortFilterProxyModel::rowsRemoved, this, &ModalTasksDialog::numberOfModalTasksChanged);

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

    QCoreApplication::processEvents();

    setFixedHeight(sizeHint().height());

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
