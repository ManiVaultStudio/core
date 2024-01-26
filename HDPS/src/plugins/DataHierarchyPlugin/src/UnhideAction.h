// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <actions/GroupAction.h>
#include <actions/TreeAction.h>

#include <models/DataHierarchyListModel.h>
#include <models/DataHierarchyFilterModel.h>

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

private:
    mv::DataHierarchyListModel      _listModel;         /** Model containing data to be displayed in the list */
    mv::DataHierarchyFilterModel    _filterModel;       /** Data hierarchy filter model */
    mv::gui::TreeAction             _treeAction;        /** Tree action for displaying the model */
};
