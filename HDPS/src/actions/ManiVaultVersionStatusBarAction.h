// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/StatusBarAction.h"
#include "actions/HorizontalGroupAction.h"
#include "actions/StringAction.h"
#include "actions/TriggerAction.h"

namespace mv::gui {

/**
 * Plugins status bar action class
 *
 * Status bar action class for displaying loaded plugins
 *
 * Note: This action is developed for internal use (not meant to be used in third-party plugins)
 * 
 * @author Thomas Kroes
 */
class ManiVaultVersionStatusBarAction : public StatusBarAction
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent object and \p title
     * @param tasksModel Reference to underlying tasks model
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param taskScope Scope of the task
     */
    ManiVaultVersionStatusBarAction(QObject* parent, const QString& title);

private:
    HorizontalGroupAction   _barGroupAction;
    StringAction            _infoAction;        /** String action displays the last record message from the model (if any) */
};

}
