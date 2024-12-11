// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "PageActionsWidget.h"

#include <QDialog>

/**
 * New project dialog class
 * 
 * Dialog class for creating new projects.
 * 
 * @author Thomas Kroes
 */
class NewProjectDialog final : public QDialog
{
public:

    /**
     * Construct a dialog with \p parent
     * @param parent Pointer to parent widget
     */
    NewProjectDialog(QWidget* parent = nullptr);

    /** Get preferred size */
    QSize sizeHint() const override {
        return QSize(600, 400);
    }

    /** Get minimum size hint*/
    QSize minimumSizeHint() const override {
        return sizeHint();
    }

private:
    PageActionsWidget           _workspacesWidget;          /** Actions widget for selecting a workspace and creating a project */
    mv::gui::TriggerAction      _cancelAction;              /** Action for exiting the dialog without creating a new project */
};
