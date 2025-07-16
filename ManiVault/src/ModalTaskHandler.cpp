// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ModalTaskHandler.h"
#include "Application.h"

#include <QMainWindow>
#include <QScreen>

#ifdef _DEBUG
    //#define MODAL_TASK_HANDLER_VERBOSE
#endif

namespace mv {

using namespace gui;
using namespace util;

ModalTaskHandler::ModalTaskHandler(QObject* parent) :
    AbstractTaskHandler(parent, nullptr),
    _modalTasksDialog(this)
{
    setMinimumDuration(0);
    setEnabled(false);

    _filterModel.setSourceModel(&_model);
    _filterModel.getTaskScopeFilterAction().setSelectedOptions({ "Modal" });
    _filterModel.getTaskStatusFilterAction().setSelectedOptions({ "Running", "Running Indeterminate", "Finished", "Aborting" });
    _filterModel.getTopLevelTasksOnlyAction().setChecked(true);

    _minimumDurationTimer.setSingleShot(true);

    const auto updateMinimumDurationTimer = [this]() -> void {
        _minimumDurationTimer.setInterval(getMinimumDuration());
    };

    updateMinimumDurationTimer();

    connect(this, &AbstractTaskHandler::minimumDurationChanged, this, updateMinimumDurationTimer);

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

    connect(&_filterModel, &QSortFilterProxyModel::rowsInserted, this, updateVisibilityDeferred);
    connect(&_filterModel, &QSortFilterProxyModel::rowsRemoved, this, updateVisibilityDeferred);
}

void ModalTaskHandler::updateDialogVisibility()
{
    if (!getEnabled())
        return;

    const auto numberOfModalTasks = getFilterModel().rowCount();

#ifdef MODAL_TASK_HANDLER_VERBOSE
    qDebug() << __FUNCTION__ << numberOfModalTasks;
#endif

    if (numberOfModalTasks == 0 && _modalTasksDialog.isVisible())
        _modalTasksDialog.close();

    if (numberOfModalTasks >= 1 && !_modalTasksDialog.isVisible()) {
        auto mainWindow = Application::getMainWindow();
        auto mainScreen = QGuiApplication::primaryScreen();

        const auto mainWindowGeometry = mainWindow ? mainWindow->geometry() : mainScreen->geometry();

        _modalTasksDialog.move(mainWindowGeometry.center() - _modalTasksDialog.rect().center());
        _modalTasksDialog.show();
    }

    QCoreApplication::processEvents();
}

TasksTreeModel& ModalTaskHandler::getModel()
{
    return _model;
}

TasksFilterModel& ModalTaskHandler::getFilterModel()
{
    return _filterModel;
}

ModalTaskHandler::ModalTasksDialog::ModalTasksDialog(ModalTaskHandler* modalTaskHandler, QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _modalTaskHandler(modalTaskHandler),
    _tasksAction(this, "Modal Tasks")
{
    setFixedWidth(1000);

    setWindowModality(Qt::ApplicationModal);
    setWindowFlag(Qt::Dialog);
    setWindowFlag(Qt::WindowCloseButtonHint, false);
    setWindowFlag(Qt::WindowTitleHint);
    //setWindowFlag(Qt::WindowStaysOnTopHint);

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);

    _tasksAction.initialize(&modalTaskHandler->getModel(), &modalTaskHandler->getFilterModel(), "Modal Task");

    auto layout = new QVBoxLayout();

    layout->addWidget(_tasksAction.createWidget(this));

    setLayout(layout);

    updateWindowTitleAndIcon();

    connect(&_modalTaskHandler->getFilterModel(), &QSortFilterProxyModel::rowsInserted, this, &ModalTasksDialog::updateWindowTitleAndIcon);
    connect(&_modalTaskHandler->getFilterModel(), &QSortFilterProxyModel::rowsRemoved, this, &ModalTasksDialog::updateWindowTitleAndIcon);

    connect(&_modalTaskHandler->getFilterModel(), &QSortFilterProxyModel::rowsInserted, this, &ModalTasksDialog::adjustSize);
    connect(&_modalTaskHandler->getFilterModel(), &QSortFilterProxyModel::rowsRemoved, this, &ModalTasksDialog::adjustSize);
    connect(&_modalTaskHandler->getFilterModel(), &QSortFilterProxyModel::layoutChanged, this, &ModalTasksDialog::adjustSize);
}

void ModalTaskHandler::ModalTasksDialog::updateWindowTitleAndIcon()
{
    auto& tasksFilterModel = _modalTaskHandler->getFilterModel();

    const auto numberOfModalTasks = tasksFilterModel.rowCount();

    const auto clockIcon = StyledIcon("clock");

    if (numberOfModalTasks == 1) {
        const auto sourceModelIndex = tasksFilterModel.mapToSource(tasksFilterModel.index(0, 0));
        const auto item             = dynamic_cast<AbstractTasksModel::Item*>(_modalTaskHandler->getModel().itemFromIndex(sourceModelIndex));
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
