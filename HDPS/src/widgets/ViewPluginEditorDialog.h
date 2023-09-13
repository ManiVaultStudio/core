// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ActionsWidget.h"

#include "models/ActionsListModel.h"
#include "models/ActionsHierarchyModel.h"

#include "actions/GroupsAction.h"
#include "actions/GroupAction.h"

#include <QDialog>

namespace hdps {
    namespace plugin {
        class ViewPlugin;
    }
}

namespace hdps::gui {

/**
 * View plugin editor dialog class
 * 
 * Dialog class for editing view plugin properties such as the actions and docking properties
 *
 * @author Thomas Kroes
 */
class ViewPluginEditorDialog : public QDialog
{
public:

    /**
     * Constructor for plugin-attached actions
     * @param parent Pointer to parent widget
     * @param viewPlugin Pointer to view plugin to edit
     */
    ViewPluginEditorDialog(QWidget* parent, hdps::plugin::ViewPlugin* viewPlugin);

    /**
     * Constructor for dataset-attached actions
     * @param parent Pointer to parent widget
     * @param rootAction Pointer to parent action of all actions to be displayed
     */
    ViewPluginEditorDialog(QWidget* parent, hdps::gui::WidgetAction* rootAction);

    /** Get preferred size */
    QSize sizeHint() const override {
        return QSize(800, 600);
    }

    /** Get minimum size hint*/
    QSize minimumSizeHint() const override {
        return sizeHint();
    }

private:
    /** Setup function common to all constructor */
    void init();

private:
    GroupsAction            _groupsAction;              /** Groups action */
    ActionsListModel        _actionsListModel;          /** List actions model of the view plugin action and its descendants */
    ActionsHierarchyModel   _actionsHierarchyModel;     /** Hierarchical actions model of the view plugin action and its descendants */
    ActionsWidget           _actionsWidget;             /** Actions widget (displays the actions hierarchy model) */
    GroupAction             _settingsAction;            /** Group action for miscellaneous settings actions */
};

}