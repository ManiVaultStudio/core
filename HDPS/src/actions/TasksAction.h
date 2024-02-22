// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/GroupAction.h"
#include "actions/TreeAction.h"
#include "actions/TriggerAction.h"

#include "models/TasksFilterModel.h"

#include "widgets/HierarchyWidget.h"

#include <QPixmap>

namespace mv::gui {

/**
 * Tasks action class
 *
 * Action class for managing tasks
 *
 * Note: This action is primarily developed for internal use (not meant to be used in third-party plugins)
 * 
 * @author Thomas Kroes
 */
class TasksAction : public GroupAction
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE TasksAction(QObject* parent, const QString& title);

    /**
     * Get tasks model
     * @return Pointer to the tasks model
     */
    AbstractTasksModel* getModel();

    /**
     * Get tasks filter model
     * @return Pointer to the tasks filter model
     */
    TasksFilterModel* getFilterModel();

    /**
     * Initialize the action with pointers to tasks \p model and \p filterModel
     * @param model Pointer to the tasks model
     * @param filterModel Pointer to the tasks filter model
     * @param itemTypeName Item type name string
     * @param mayLoadTasksPlugin When set to true, adds a trigger action to the toolbar for loading the tasks plugin
     */
    void initialize(AbstractTasksModel* model, TasksFilterModel* filterModel, const QString& itemTypeName, bool mayLoadTasksPlugin = true);

private:

    /**
     * Open any progress editor which has not been opened yet in the filtered tasks view recursively
     * @param itemView Item view to set the persistent editors for
     * @param parent Parent model index
     */
    void openPersistentProgressEditorsRecursively(QAbstractItemView& itemView, const QModelIndex& parent = QModelIndex());

    /**
     * Close any progress editor in the filtered tasks view recursively
     * @param itemView Item view to set the persistent editors for
     * @param parent Parent model index
     */
    void closePersistentProgressEditorsRecursively(QAbstractItemView& itemView, const QModelIndex& parent = QModelIndex());

    /**
     * get whether any aggregate tasks are displayed
     * @return Boolean determining whether there are any aggregate tasks are displayed
     */
    bool hasAgregateTasks() const;

signals:

    /**
     * Signals that auto hide kill column changed to 
     * @param autoHideKillCollumn Boolean determining whether the kill column is automatically hidden when there are no killable tasks
     */
    void autoHideKillCollumnChanged(bool autoHideKillCollumn);

private:
    AbstractTasksModel*     _model;                     /** Tasks model */
    TasksFilterModel*       _filterModel;               /** Filter model for tasks model */
    TreeAction              _treeAction;                /** Action to displays the tasks */
    bool                    _mayLoadTasksPlugin;        /** When set to true, adds a trigger action to the toolbar for loading the tasks plugin */
    TriggerAction           _loadTasksPluginAction;     /** Triggers loading the tasks plugin */
};

}

Q_DECLARE_METATYPE(mv::gui::TasksAction)

inline const auto tasksActionMetaTypeId = qRegisterMetaType<mv::gui::TasksAction*>("mv::gui::TasksAction");
