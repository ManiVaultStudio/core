// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "StatusBarAction.h"

#include <actions/HorizontalGroupAction.h>
#include <actions/StringAction.h>

/**
 * ManiVault version status bar action class
 *
 * Status bar action class for displaying the ManiVault version on the status bar
 * and additional ManiVault info in a popup
 *
 * @author Thomas Kroes
 */
class ManiVaultVersionStatusBarAction : public StatusBarAction
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    ManiVaultVersionStatusBarAction(QObject* parent, const QString& title);

private:
    mv::gui::HorizontalGroupAction  _barGroupAction;    /** Bar group action */
    mv::gui::StringAction           _iconAction;        /** String action for showing a home icon with FontAwesome */
    mv::gui::StringAction           _versionAction;     /** String action that displays the ManiVault version on the status bar */
    mv::gui::HorizontalGroupAction  _popupGroupAction;  /** Popup group action */
    mv::gui::StringAction           _popupAction;       /** Popup string action for ManiVault info */
};
