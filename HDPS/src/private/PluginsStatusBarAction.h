// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "StatusBarAction.h"

#include <actions/TriggerAction.h>
#include <actions/TreeAction.h>

#include <models/PluginFactoriesTreeModel.h>
#include <models/PluginFactoriesListModel.h>
#include <models/PluginFactoriesFilterModel.h>

/**
 * Plugins status bar action class
 *
 * Status bar action class for displaying loaded plugins
 *
 * @author Thomas Kroes
 */
class PluginsStatusBarAction : public StatusBarAction
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    PluginsStatusBarAction(QObject* parent, const QString& title);

private:
    mv::gui::TriggerAction              _loadPluginBrowserAction;   /** Triggers loading the logging plugin */
    mv::PluginFactoriesTreeModel        _treeModel;                 /** Plugins tree model */
    mv::PluginFactoriesListModel        _listModel;                 /** Plugins list model */
    mv::PluginFactoriesFilterModel      _filterModel;               /** Filter model for the plugins model */
    mv::gui::TreeAction                 _pluginsAction;             /** Tree action plugins listing in popup group action */
};
