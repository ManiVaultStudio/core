// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <actions/StatusBarAction.h>
#include <actions/ToggleAction.h>

namespace mv::gui {

/**
 * Plugin status bar action class
 *
 * Extends the status bar action class with plugin specific features
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT PluginStatusBarAction : public StatusBarAction
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent object, \p title and possibly a FontAwesome \p icon
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param pluginKind Plugin kind to use (icon will be retrieved from the plugin factory when the plugin kind is valid)
     */
    PluginStatusBarAction(QObject* parent, const QString& title, const QString& pluginKind = "");

private:

    /** Show/hide depending on the rule */
    void updateConditionalVisibility();

public: // Action getters

    ToggleAction& getConditionallyVisibleAction() { return _conditionallyVisibleAction; }

private:
    const QString   _pluginKind;                    /** Plugin kind this status bar action represents */
    ToggleAction    _conditionallyVisibleAction;    /** Always show when checked and only show the plugin status bar when there is at least one plugin active when unchecked */
};

}