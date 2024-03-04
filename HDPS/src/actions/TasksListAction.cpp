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
    setIconByName("tasks");
    setDefaultWidgetFlag(NoMargins);

    _loadTasksPluginAction.setIconByName("window-maximize");
    _loadTasksPluginAction.setDefaultWidgetFlags(TriggerAction::WidgetFlag::Icon);
    _loadTasksPluginAction.setToolTip("Load tasks plugin");

    connect(&_loadTasksPluginAction, &TriggerAction::triggered, this, [this]() -> void {
        mv::plugins().requestViewPlugin("Tasks", nullptr, DockAreaFlag::Bottom);

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

}