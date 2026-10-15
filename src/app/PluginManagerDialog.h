// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <models/PluginsTreeModel.h>
#include <models/PluginsFilterModel.h>

#include <widgets/HierarchyWidget.h>

#include <actions/TriggerAction.h>

#include <QDialog>

/**
 * Plugin manager dialog class
 * 
 * Dialog class for inspecting the loaded plugins and the created plugin instances.
 * 
 * It shows plugin:
 * - Types at the top-level
 *     - Factories below the plugin types
 *         - Instances for each plugin factory   
 *
 * It also allows plugins to be destroyed (available in right-click menu).
 * 
 * @author Thomas Kroes
 */
class PluginManagerDialog : public QDialog
{
public:

    /**
     * Construct a plugin manager dialog with \p parent
     * @param parent Pointer to parent widget
     */
    PluginManagerDialog(QWidget* parent = nullptr);

    /** Get preferred size */
    QSize sizeHint() const override {
        return QSize(700, 500);
    }

    /** Get minimum size hint*/
    QSize minimumSizeHint() const override {
        return sizeHint();
    }

    /** Show the plugin manager dialog modally */
    static void create();

private:
    mv::PluginsTreeModel        _model;                 /** Plugins tree model (interfaces with a plugin manager) */
    mv::PluginsFilterModel      _filterModel;           /** Sorting and filtering model for the plugin manager model */
    mv::gui::HierarchyWidget    _hierarchyWidget;       /** Widget for displaying the loaded plugins */
    mv::gui::TriggerAction      _okAction;              /** Action for exiting the dialog */
};
