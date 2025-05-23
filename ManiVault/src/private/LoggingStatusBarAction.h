// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <actions/StatusBarAction.h>
#include <actions/StringAction.h>
#include <actions/TreeAction.h>
#include <actions/TriggerAction.h>

#include <models/LoggingModel.h>
#include <models/LoggingFilterModel.h>

/**
 * Logging status bar action class
 *
 * Logging action class for display and interaction with logging records
 *
 * @author Thomas Kroes
 */
class LoggingStatusBarAction : public mv::gui::StatusBarAction
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    LoggingStatusBarAction(QObject* parent, const QString& title);

private:
    mv::LoggingModel            _model;                 /** Model of the tasks model */
    mv::LoggingFilterModel      _filterModel;           /** Filter model of the logging model */
    mv::gui::StringAction       _lastMessageAction;     /** String action displays the last record message from the model (if any) */
    mv::gui::TreeAction         _recordsAction;         /** Tree action which displays the filtered records from the filter model */
    mv::gui::TriggerAction      _clearRecordsAction;    /** Clears the model records when triggered */
    mv::gui::TriggerAction      _loadPluginAction;      /** Triggers loading the logging plugin */
};
