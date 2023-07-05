// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QDialog>

#include <actions/GroupAction.h>
#include <actions/TriggersAction.h>
#include <actions/TriggerAction.h>

/**
 * Project settings dialog class
 * 
 * Dialog class for editing project settings.
 * 
 * @author Thomas Kroes
 */
class ProjectSettingsDialog : public QDialog
{
public:

    /**
     * Construct a project settings dialog with \p parent
     * @param parent Pointer to parent widget
     */
    ProjectSettingsDialog(QWidget* parent = nullptr);

    /** Get preferred size */
    QSize sizeHint() const override {
        return QSize(600, 500);
    }

    /** Get minimum size hint*/
    QSize minimumSizeHint() const override {
        return sizeHint();
    }

private:
    hdps::gui::GroupAction      _groupAction;       /** Settings group action */
    hdps::gui::TriggerAction    _okAction;          /** Action for exiting the dialog */
};
