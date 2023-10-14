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

namespace mv {

using namespace gui;

ModalTaskHandler::ModalTaskHandler(QObject* parent) :
    AbstractTaskHandler(parent, nullptr),
    _tasksFilterModel(),
    _modalTasksDialog(this)
{
    setMinimumDuration(0);
    setEnabled(false);

    _tasksFilterModel.setSourceModel(tasks().getTreeModel());

    _minimumDurationTimer.setSingleShot(true);
    
    const auto updateMinimumDurationTimer = [this]() -> void {
        _minimumDurationTimer.setInterval(getMinimumDuration());
    };

    updateMinimumDurationTimer();

    connect(this, &AbstractTaskHandler::minimumDurationChanged, this, updateMinimumDurationTimer);

    _tasksFilterModel.getTaskStatusFilterAction().setSelectedOptions({ "Running", "Running Indeterminate", "Finished", "Aborting" });
    _tasksFilterModel.getTaskScopeFilterAction().setSelectedOptions({ "Modal" });

    const auto updateVisibilityDeferred = [this]() -> void {
        if (_modalTasksDialog.isHidden()) {
            if (!_minimumDurationTimer.isActive())
                _minimumDurationTimer.start();
        }
        else {
            updateDialogVisibility();
        }
    };

    connect(&_minimumDurationTimer, &QTimer::timeout, this, &ModalTaskHandler::updateDialogVisibility);

    connect(&_tasksFilterModel, &QSortFilterProxyModel::rowsInserted, this, [this, updateVisibilityDeferred](const QModelIndex& parent, int first, int last) -> void {
        if (parent == QModelIndex())
            updateVisibilityDeferred();
    });

    connect(&_tasksFilterModel, &QSortFilterProxyModel::rowsRemoved, this, [this, updateVisibilityDeferred](const QModelIndex& parent, int first, int last) -> void {
        if (parent == QModelIndex())
            updateVisibilityDeferred();
    });
}

void ModalTaskHandler::updateDialogVisibility()
{
    if (!getEnabled())
        return;

    const auto numberOfModalTasks = getTasksFilterModel().rowCount();

#ifdef MODAL_TASK_HANDLER_VERBOSE
    qDebug() << __FUNCTION__ << numberOfModalTasks;
#endif

    if (numberOfModalTasks == 0 && _modalTasksDialog.isVisible())
        _modalTasksDialog.close();

    if (numberOfModalTasks >= 1 && !_modalTasksDialog.isVisible())
        _modalTasksDialog.show();

    QCoreApplication::processEvents();
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
    
    setLayout(new QGridLayout());
    
    auto& tasksFilterModel = _modalTaskHandler->getTasksFilterModel();

    connect(&tasksFilterModel, &QSortFilterProxyModel::rowsInserted, this, [this](const QModelIndex& parent, int first, int last) -> void {
        if (parent == QModelIndex())
            updateLayout();
    });

    connect(&tasksFilterModel, &QSortFilterProxyModel::rowsRemoved, this, [this](const QModelIndex& parent, int first, int last) -> void {
        if (parent == QModelIndex())
            updateLayout();
    });

    updateLayout();
    updateWindowTitleAndIcon();
}

void ModalTaskHandler::ModalTasksDialog::updateLayout()
{
    updateWindowTitleAndIcon();

    auto& tasksFilterModel = _modalTaskHandler->getTasksFilterModel();

    const auto numberOfModalTasks = tasksFilterModel.rowCount();

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

        if (!_widgetsMap.contains(progressItem->getTask())) {
            auto labelWidget    = new QLabel(progressItem->getTask()->getName() + ":");
            auto progressWidget = progressItem->getTaskAction().createWidget(this);

            progressWidget->setFixedHeight(25);

            _widgetsMap[progressItem->getTask()] = { labelWidget, progressWidget };
        }

        auto gridLayout = static_cast<QGridLayout*>(layout());

        const auto rowCount = gridLayout->rowCount();

        gridLayout->addWidget(_widgetsMap[progressItem->getTask()][0], rowCount, 0);
        gridLayout->addWidget(_widgetsMap[progressItem->getTask()][1], rowCount, 1);
    }

    for (auto task : _widgetsMap.keys()) {
        if (currentTasks.contains(task))
            continue;

        for (auto widget : _widgetsMap[task])
            delete widget;

        _widgetsMap.remove(task);
    }

    adjustSize();

    QCoreApplication::processEvents();

    setFixedHeight(sizeHint().height());
}

void ModalTaskHandler::ModalTasksDialog::cleanLayout()
{
    QLayoutItem* item;

    while ((item = this->layout()->takeAt(0)) != 0)
        delete item;
}

void ModalTaskHandler::ModalTasksDialog::updateWindowTitleAndIcon()
{
    auto& tasksFilterModel = _modalTaskHandler->getTasksFilterModel();

    const auto numberOfModalTasks = tasksFilterModel.rowCount();

    const auto clockIcon = Application::getIconFont("FontAwesome").getIcon("clock");

    if (numberOfModalTasks == 1) {
        const auto sourceModelIndex = tasksFilterModel.mapToSource(tasksFilterModel.index(0, 0));
        const auto item             = dynamic_cast<AbstractTasksModel::Item*>(tasks().getTreeModel()->itemFromIndex(sourceModelIndex));
        const auto task             = item->getTask();
        const auto taskName         = task->getName();
        const auto taskIcon         = task->getIcon();

        setWindowTitle(QString("Waiting for %1 to complete...").arg(taskName));
        setWindowIcon(taskIcon.isNull() ? clockIcon : taskIcon);
    }
    else {
        setWindowTitle(QString("Waiting for %1 tasks to complete...").arg(QString::number(numberOfModalTasks)));
        setWindowIcon(clockIcon);
    }
}

}
