// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <actions/StatusBarAction.h>
#include <actions/TriggerAction.h>
#include <actions/GroupAction.h>

/**
 * Workspace status bar action class
 *
 * Status bar action class for displaying workspace settings
 *
 * @author Thomas Kroes
 */
class WorkspaceStatusBarAction : public mv::gui::StatusBarAction
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    WorkspaceStatusBarAction(QObject* parent, const QString& title);

private:
    mv::gui::TriggerAction  _loadPluginBrowserAction;   /** Triggers loading the logging plugin */
    mv::gui::GroupAction    _settingsAction;
};
