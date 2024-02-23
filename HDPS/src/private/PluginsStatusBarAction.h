// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "StatusBarAction.h"

#include <actions/HorizontalGroupAction.h>
#include <actions/StringAction.h>
#include <actions/TriggerAction.h>

/**
 * Plugins status bar action class
 *
 * Status bar action class for displaying loaded plugins
 *
 * @author Thomas Kroes
 */
class PluginsStatusBarAction : public StatusBarAction
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    PluginsStatusBarAction(QObject* parent, const QString& title);

private:
    mv::gui::HorizontalGroupAction  _barGroupAction;        /** Bar group action */
    mv::gui::StringAction           _lastMessageAction;     /** String action displays the last record message from the model (if any) */
    mv::gui::TriggerAction          _loadPluginAction;      /** Triggers loading the logging plugin */
};
