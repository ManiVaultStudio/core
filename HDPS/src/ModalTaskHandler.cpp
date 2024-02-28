// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ModalTaskHandler.h"
#include "ModalTask.h"
#include "Application.h"
#include "CoreInterface.h"

#include <QMainWindow>

#ifdef _DEBUG
    //#define MODAL_TASK_HANDLER_VERBOSE
#endif

namespace mv {

using namespace gui;

ModalTaskHandler::ModalTaskHandler(QObject* parent) :
    AbstractTaskHandler(parent, nullptr),
    _model(),
    _filterModel(),
    _modalTasksDialog(this)
{
    setMinimumDuration(0);
    setEnabled(false);

    _filterModel.setSourceModel(&_model);
    _filterModel.getTopLevelTasksOnlyAction().setChecked(true);

    _minimumDurationTimer.setSingleShot(true);
    
    const auto updateMinimumDurationTimer = [this]() -> void {
        _minimumDurationTimer.setInterval(getMinimumDuration());
    };

    updateMinimumDurationTimer();

    connect(this, &AbstractTaskHandler::minimumDurationChanged, this, updateMinimumDurationTimer);

    _filterModel.getTaskStatusFilterAction().setSelectedOptions({ "Running", "Running Indeterminate", "Finished", "Aborting" });
    _filterModel.getTaskScopeFilterAction().setSelectedOptions({ "Modal" });

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

    connect(&_filterModel, &QSortFilterProxyModel::rowsInserted, this, [this, updateVisibilityDeferred](const QModelIndex& parent, int first, int last) -> void {
        if (parent == QModelIndex())
            updateVisibilityDeferred();
    });

    connect(&_filterModel, &QSortFilterProxyModel::rowsRemoved, this, [this, updateVisibilityDeferred](const QModelIndex& parent, int first, int last) -> void {
        if (parent == QModelIndex())
            updateVisibilityDeferred();
    });
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
        const auto mainWindowGeometry = Application::getMainWindow()->geometry();

        _modalTasksDialog.move(mainWindowGeometry.center() - _modalTasksDialog.rect().center());
        _modalTasksDialog.show();
    }

    QCoreApplication::processEvents();
}

TasksListModel& ModalTaskHandler::getModel()
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
    setWindowModality(Qt::ApplicationModal);

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    setFixedWidth(1000);

    setWindowFlag(Qt::Dialog);
    setWindowFlag(Qt::WindowCloseButtonHint, false);
    setWindowFlag(Qt::WindowTitleHint);
    setWindowFlag(Qt::WindowStaysOnTopHint);
    //setWindowFlag(Qt::SubWindow);

    _tasksAction.setWidgetConfigurationFunction([this](WidgetAction* action, QWidget* widget) -> void {
        auto hierarchyWidget = widget->findChild<HierarchyWidget*>("HierarchyWidget");

        Q_ASSERT(hierarchyWidget);

        if (hierarchyWidget == nullptr)
            return;

        hierarchyWidget->getToolbarAction().setVisible(false);
        hierarchyWidget->setHeaderHidden(true);

        auto& treeView = hierarchyWidget->getTreeView();

        auto palette = treeView.palette();

        palette.setColor(QPalette::Base, QApplication::palette().color(QPalette::Normal, QPalette::Window));

        treeView.setAutoFillBackground(true);
        treeView.setFrameShape(QFrame::NoFrame);
        treeView.setPalette(palette);
        treeView.viewport()->setPalette(palette);
        treeView.setRootIsDecorated(false);

        treeView.setColumnHidden(static_cast<int>(AbstractTasksModel::Column::Status), true);
        treeView.setColumnHidden(static_cast<int>(AbstractTasksModel::Column::Type), true);

        const auto numberOfModalTasksChanged = [this, &treeView, hierarchyWidget, widget]() -> void {
            const auto numberOfTasks = _modalTaskHandler->getFilterModel().rowCount();

            std::int32_t height = 0;

            for (int rowIndex = 0; rowIndex < numberOfTasks; ++rowIndex)
                height += treeView.sizeHintForRow(rowIndex);

            hierarchyWidget->setFixedHeight(height);

            treeView.setColumnHidden(static_cast<int>(AbstractTasksModel::Column::Kill), !_modalTaskHandler->getFilterModel().hasKillableTasks());

            Application::processEvents();
        };

        numberOfModalTasksChanged();

        connect(&_modalTaskHandler->getFilterModel(), &QSortFilterProxyModel::rowsInserted, &treeView, numberOfModalTasksChanged);
        connect(&_modalTaskHandler->getFilterModel(), &QSortFilterProxyModel::rowsRemoved, &treeView, numberOfModalTasksChanged);
    });
    _tasksAction.initialize(&modalTaskHandler->getModel(), &modalTaskHandler->getFilterModel(), "Modal Task");

    auto layout = new QVBoxLayout();

    layout->addWidget(_tasksAction.createWidget(this));

    setLayout(layout);
    
    updateWindowTitleAndIcon();

    connect(&_modalTaskHandler->getFilterModel(), &QSortFilterProxyModel::rowsInserted, this, &ModalTasksDialog::updateWindowTitleAndIcon);
    connect(&_modalTaskHandler->getFilterModel(), &QSortFilterProxyModel::rowsRemoved, this, &ModalTasksDialog::updateWindowTitleAndIcon);
}

void ModalTaskHandler::ModalTasksDialog::updateWindowTitleAndIcon()
{
    auto& tasksFilterModel = _modalTaskHandler->getFilterModel();

    const auto numberOfModalTasks = tasksFilterModel.rowCount();

    const auto clockIcon = Application::getIconFont("FontAwesome").getIcon("clock");

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
