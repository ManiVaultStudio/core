// SPDX-License-Identifier: LGPL-3.0-or-later Group
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <actions/GroupAction.h>
#include <actions/TreeAction.h>
#include <actions/HorizontalGroupAction.h>

#include <models/DataHierarchyTreeModel.h>
#include <models/DataHierarchyListModel.h>
#include <models/DataHierarchyFilterModel.h>

#include <QIcon>

/**
 * Un-hide action class
 *
 * Action class for un-hiding hidden datasets
 *
 * @author Thomas Kroes
 */
class UnhideAction : public mv::gui::GroupAction
{
    Q_OBJECT

public:

    /**
     * Construct with pointer to \p parent widget and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE UnhideAction(QObject* parent, const QString& title);

protected:
    /**
     * Override QObject's event handling
     * @return Boolean Whether the event was recognized and processed
     */
    bool event(QEvent* event) override;

private:
    /**
     * Updates badge and enables/disables the action
     * @return 
     */
    void updateReadOnlyAndIcon();

private:
    mv::gui::TreeAction             _treeAction;                /** Tree action for displaying the model */
    mv::DataHierarchyListModel      _listModel;                 /** List model of the data hierarchy */
    mv::DataHierarchyFilterModel    _listFilterModel;           /** Data hierarchy list filter model */
    mv::gui::HorizontalGroupAction  _triggersGroupAction;       /** Group action for hiding selected/all */
    mv::gui::TriggerAction          _selectedAction;            /** Trigger action to un-hide the selected dataset(s) */
    mv::gui::TriggerAction          _allAction;                 /** Trigger action to un-hide all datasets */
    QIcon                           _icon;                      /** Cached icon */
};
