// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QDialog>

#include <actions/GroupsAction.h>
#include <actions/GroupAction.h>

namespace mv::gui {

/**
 * Settings manager dialog class
 * 
 * Dialog class for modifying settings.
 *
 * @author Thomas Kroes
 */
class SettingsManagerDialog : public QDialog
{
public:

    /**
     * Construct a dialog for the settings manager with \p with parent widget
     * @param parent Pointer to parent widget
     */
    SettingsManagerDialog(QWidget* parent = nullptr);

    /** Get preferred size */
    QSize sizeHint() const override;

    /** Get minimum size hint*/
    QSize minimumSizeHint() const override {
        return sizeHint();
    }

private:
    GroupsAction    _groupsAction;      /** Groups action for all global settings sections */
};

}