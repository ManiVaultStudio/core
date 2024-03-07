// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QDialog>

#include <actions/GroupAction.h>
#include <actions/TriggerAction.h>

/**
 * Workspace settings class
 * 
 * Dialog class for editing workspace settings.
 * 
 * @author Thomas Kroes
 */
class WorkspaceSettingsDialog : public QDialog
{
public:

    /**
     * Construct a workspace settings dialog with \p parent
     * @param parent Pointer to parent widget
     */
    WorkspaceSettingsDialog(QWidget* parent = nullptr);

    /** Get preferred size */
    QSize sizeHint() const override {
        return QSize(600, 400);
    }

    /** Get minimum size hint*/
    QSize minimumSizeHint() const override {
        return sizeHint();
    }

private:
    mv::gui::GroupAction      _groupAction;       /** Settings group action */
    mv::gui::TriggerAction    _okAction;          /** Action for exiting the dialog */
};
