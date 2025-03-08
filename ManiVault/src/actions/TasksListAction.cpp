// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TasksListAction.h"

#include "CoreInterface.h"
#include "AbstractTaskManager.h"

#include "actions/TaskAction.h"
#include "actions/ProgressAction.h"
#include "actions/TriggerAction.h"

#include "models/TasksTreeModel.h"

#include <QStyledItemDelegate>
#include <QStyleOptionButton>
#include <QHeaderView>
#include <QSortFilterProxyModel>

namespace mv::gui {

TasksListAction::TasksListAction(QObject* parent, const QString& title) :
    GroupAction(parent, title),
    _model(nullptr),
    _filterModel(nullptr),
    _widgetConfigurationFunction(),
    _mayLoadTasksPlugin(true),
    _loadTasksPluginAction(this, "Plugin")
{
    setShowLabels(false);
    setIconByName("list-check");
    setDefaultWidgetFlag(NoMargins);

    _loadTasksPluginAction.setIconByName("window-maximize");
    _loadTasksPluginAction.setDefaultWidgetFlags(TriggerAction::WidgetFlag::Icon);
    _loadTasksPluginAction.setToolTip("Load tasks plugin");

    connect(&_loadTasksPluginAction, &TriggerAction::triggered, this, [this]() -> void {
        mv::plugins().requestViewPlugin("list-check", nullptr, DockAreaFlag::Bottom);

        _loadTasksPluginAction.setEnabled(false);
    });
}

AbstractTasksModel* TasksListAction::getModel()
{
    return _model;
}

TasksFilterModel* TasksListAction::getFilterModel()
{
    return _filterModel;
}

void TasksListAction::initialize(AbstractTasksModel* model, TasksFilterModel* filterModel, const QString& itemTypeName, WidgetConfigurationFunction widgetConfigurationFunction /*= WidgetConfigurationFunction()*/, bool mayLoadTasksPlugin /*= true*/)
{
    Q_ASSERT(model);
    Q_ASSERT(filterModel);

    if (!model || !filterModel)
        return;

    _model                          = model;
    _filterModel                    = filterModel;
    _widgetConfigurationFunction    = widgetConfigurationFunction;
    _mayLoadTasksPlugin             = mayLoadTasksPlugin;
}

TasksListAction::Widget::Widget(QWidget* parent, TasksListAction* tasksListAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, tasksListAction, widgetFlags),
    _tasksListAction(tasksListAction),
    _layout(),
    _widgetsMap()
{
    setLayout(&_layout);

    auto tasksFilterModel = _tasksListAction->getFilterModel();

    Q_ASSERT(tasksFilterModel);

    connect(tasksFilterModel, &QSortFilterProxyModel::rowsInserted, this, &Widget::updateLayout);
    connect(tasksFilterModel, &QSortFilterProxyModel::rowsRemoved, this, &Widget::updateLayout);

    updateLayout();
}

void TasksListAction::Widget::updateLayout()
{
    auto tasksModel         = _tasksListAction->getModel();
    auto tasksFilterModel   = _tasksListAction->getFilterModel();

    Q_ASSERT(tasksModel);
    Q_ASSERT(tasksFilterModel);

    const auto numberOfTasks = tasksFilterModel->rowCount();

    cleanLayout();

    QVector<Task*> currentTasks;

    for (int rowIndex = 0; rowIndex < numberOfTasks; ++rowIndex) {
        const auto sourceModelIndex = tasksFilterModel->mapToSource(tasksFilterModel->index(rowIndex, static_cast<int>(AbstractTasksModel::Column::Progress)));

        if (!sourceModelIndex.isValid())
            continue;

        auto progressItem = dynamic_cast<AbstractTasksModel::ProgressItem*>(tasksModel->itemFromIndex(sourceModelIndex));

        Q_ASSERT(progressItem != nullptr);

        if (progressItem == nullptr)
            continue;

        currentTasks << progressItem->getTask();

        if (!_widgetsMap.contains(progressItem->getTask())) {
            auto labelWidget = new QLabel(progressItem->getTask()->getName() + ":");
            auto progressWidget = progressItem->getTaskAction().createWidget(this);

            progressWidget->setFixedHeight(25);

            _widgetsMap[progressItem->getTask()] = { labelWidget, progressWidget };
        }

        const auto rowCount = _layout.rowCount();

        _layout.addWidget(_widgetsMap[progressItem->getTask()][0], rowCount, 0);
        _layout.addWidget(_widgetsMap[progressItem->getTask()][1], rowCount, 1);
    }

    for (auto task : _widgetsMap.keys()) {
        if (currentTasks.contains(task))
            continue;

        for (auto widget : _widgetsMap[task])
            delete widget;

        _widgetsMap.remove(task);
    }

    Application::processEvents();

    updateGeometry();
    adjustSize();
}

void TasksListAction::Widget::cleanLayout()
{
    QLayoutItem* item;

    while ((item = _layout.takeAt(0)) != 0)
        delete item;
}

}