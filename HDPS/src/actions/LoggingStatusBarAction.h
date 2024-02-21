// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/HorizontalGroupAction.h"
#include "actions/StringAction.h"
#include "actions/TreeAction.h"
#include "actions/TriggerAction.h"

#include "models/LoggingModel.h"
#include "models/LoggingFilterModel.h"

namespace mv::gui {

/**
 * Logging status bar action class
 *
 * Logging action class for display and interaction with logging records
 *
 * @author Thomas Kroes
 */
class LoggingStatusBarAction : public HorizontalGroupAction
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent object and \p title
     * @param tasksModel Reference to underlying tasks model
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param taskScope Scope of the task
     */
    LoggingStatusBarAction(QObject* parent, const QString& title);

    /**
     * Get tasks model
     * @return Reference to tasks model
     */
    //AbstractTasksModel& getTasksModel();

    /**
     * Get tasks filter model
     * @return Reference to tasks filter model
     */
    //TasksFilterModel& getTasksFilterModel();

signals:

    /**
     * Signals that popup force hidden to \p popupForceHidden
     * @param popupForceHidden Boolean determining whether popup force hidden is on or not
     */
    //void popupForceHiddenChanged(bool popupForceHidden);

private:
    LoggingModel        _model;                 /** Model of the tasks model */
    LoggingFilterModel  _filterModel;           /** Filter model of the logging model */
    StringAction        _lastMessageAction;     /** String action displays the last record message from the model (if any) */
    TreeAction          _recordsAction;         /** Tree action which displays the filtered records from the filter model */
    TriggerAction       _loadPluginAction;      /** Triggers loading the logging plugin */
};

}
