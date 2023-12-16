// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <actions/ToggleAction.h>
#include <actions/TriggerAction.h>
#include <actions/HorizontalGroupAction.h>

#include <QDialog>

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
     * Construct with \p parent widget
     * @param parent Pointer to parent widget
     */
    ConfirmDatasetsRemovalDialog(QWidget* parent);

    /** Get preferred size */
    QSize sizeHint() const override {
        return QSize(350, 150);
    }

    /** Get minimum size hint*/
    QSize minimumSizeHint() const override {
        return sizeHint();
    }

protected:
    //ToggleAction    _alwaysAskPermissionAction;       /** Whether to always ask permission prior to removing datasets */
    TriggerAction           _removeAction;              /** Trigger action to remove the dataset (s) */
    TriggerAction           _cancelAction;              /** Trigger action to cancel the removal and the dialog */
    HorizontalGroupAction   _buttonsGroupAction;        /** Group action to layout out the previous buttons horizontally */
};
