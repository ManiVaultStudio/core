// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ModalTaskHandler.h"
#include "TasksTreeModel.h"
#include "ModalTask.h"

#include "CoreInterface.h"

#ifdef _DEBUG
    //#define MODAL_TASK_HANDLER_VERBOSE
#endif

namespace hdps {

using namespace gui;

ModalTaskHandler::ModalTaskHandler(QObject* parent) :
    AbstractTaskHandler(parent, nullptr),
    _tasksFilterModel(),
    _modalTasksDialog(this)
{
    _tasksFilterModel.setSourceModel(tasks().getTreeModel());

    _minimumDurationTimer.setSingleShot(true);
    
    const auto updateMinimumDurationTimer = [this]() -> void {
        _minimumDurationTimer.setInterval(getMinimumDuration());
    };

    updateMinimumDurationTimer();

    connect(this, &AbstractTaskHandler::minimumDurationChanged, this, updateMinimumDurationTimer);

    _tasksFilterModel.getTaskStatusFilterAction().setSelectedOptions({ "Running", "Running Indeterminate", "Finished" });
    _tasksFilterModel.getTaskScopeFilterAction().setSelectedOptions({ "Modal" });

    const auto updateVisibilityDeferred = [this]() -> void {
        if (_modalTasksDialog.isHidden() && !_minimumDurationTimer.isActive())
            _minimumDurationTimer.start();

        if (_modalTasksDialog.isVisible())
            updateDialogVisibility();
    };

    connect(&_minimumDurationTimer, &QTimer::timeout, this, &ModalTaskHandler::updateDialogVisibility);

    connect(&_tasksFilterModel, &QSortFilterProxyModel::layoutChanged, this, updateVisibilityDeferred);
    connect(&_tasksFilterModel, &QSortFilterProxyModel::rowsRemoved, this, updateVisibilityDeferred);
}

void ModalTaskHandler::updateDialogVisibility()
{
    const auto numberOfModalTasks = getTasksFilterModel().rowCount();

    qDebug() << __FUNCTION__ << numberOfModalTasks;

    if (numberOfModalTasks == 0 && _modalTasksDialog.isVisible())
        _modalTasksDialog.close();

    if (numberOfModalTasks >= 1 && !_modalTasksDialog.isVisible())
        _modalTasksDialog.show();
}

TasksFilterModel& ModalTaskHandler::getTasksFilterModel()
{
    return _tasksFilterModel;
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
    
    auto& tasksFilterModel = _modalTaskHandler->getTasksFilterModel();

    connect(&tasksFilterModel, &QSortFilterProxyModel::layoutChanged, this, &ModalTasksDialog::numberOfModalTasksChanged);
    connect(&tasksFilterModel, &QSortFilterProxyModel::rowsInserted, this, &ModalTasksDialog::numberOfModalTasksChanged);
    connect(&tasksFilterModel, &QSortFilterProxyModel::rowsRemoved, this, &ModalTasksDialog::numberOfModalTasksChanged);

    numberOfModalTasksChanged();
}

void ModalTaskHandler::ModalTasksDialog::numberOfModalTasksChanged()
{
    auto& tasksFilterModel = _modalTaskHandler->getTasksFilterModel();

    const auto numberOfModalTasks = tasksFilterModel.rowCount();

    if (numberOfModalTasks == 0)
        return;

    cleanLayout();

    QVector<Task*> currentTasks;

    for (int rowIndex = 0; rowIndex < numberOfModalTasks; ++rowIndex) {
        const auto sourceModelIndex = tasksFilterModel.mapToSource(tasksFilterModel.index(rowIndex, static_cast<int>(AbstractTasksModel::Column::Progress)));

        if (!sourceModelIndex.isValid())
            continue;

        auto progressItem = dynamic_cast<AbstractTasksModel::ProgressItem*>(tasks().getTreeModel()->itemFromIndex(sourceModelIndex));

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
        const auto item             = dynamic_cast<AbstractTasksModel::Item*>(tasks().getTreeModel()->itemFromIndex(sourceModelIndex));
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
