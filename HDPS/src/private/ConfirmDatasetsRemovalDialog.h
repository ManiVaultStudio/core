// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <actions/ToggleAction.h>
#include <actions/TriggerAction.h>
#include <actions/HorizontalGroupAction.h>

#include <widgets/HierarchyWidget.h>

#include <QDialog>
#include <QStandardItemModel>

using namespace mv::gui;

/**
 * Confirm datasets removal dialog class
 *
 * Dialog for asking confirmation prior to removing of datasets
 *
 * @author Thomas Kroes
 */
class ConfirmDatasetsRemovalDialog : public QDialog
{
public:

    /**
     * Construct with pointer to \p parent widget
     * @param parent Pointer to parent widget
     */
    ConfirmDatasetsRemovalDialog(QWidget* parent = nullptr);

    /** Get preferred size */
    QSize sizeHint() const override {
        return QSize(400, 400);
    }

    /** Get minimum size hint*/
    QSize minimumSizeHint() const override {
        return sizeHint();
    }

protected:
    QStandardItemModel      _datasetsToRemoveModel;         /** Hierarchical model which contains all datasets which need to be removed */
    HierarchyWidget         _datasetsHierarchyWidget;       /** Hierarchy widget for showing the model contents */
    TriggerAction           _removeAction;                  /** Trigger action to remove the dataset (s) */
    TriggerAction           _cancelAction;                  /** Trigger action to cancel the removal and the dialog */
    HorizontalGroupAction   _buttonsGroupAction;            /** Group action to layout out the previous buttons horizontally */
};
