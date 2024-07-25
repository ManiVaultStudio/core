// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "GlobalSettingsGroupAction.h"

#include "actions/ToggleAction.h"
#include "actions/HorizontalGroupAction.h"

namespace mv::gui
{

/**
 * Miscellaneous global settings action class
 *
 * Action class which groups all miscellaneous global settings
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT MiscellaneousSettingsAction final : public GlobalSettingsGroupAction
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     */
    MiscellaneousSettingsAction(QObject* parent);

    /** Update status bar options action to reflect the current status bar items */
    void updateStatusBarOptionsAction();

public: // Action getters

    ToggleAction& getIgnoreLoadingErrorsAction() { return _ignoreLoadingErrorsAction; }
    ToggleAction& getAskConfirmationBeforeRemovingDatasetsAction() { return _askConfirmationBeforeRemovingDatasetsAction; }
    ToggleAction& getKeepDescendantsAfterRemovalAction() { return _keepDescendantsAfterRemovalAction; }
    ToggleAction& getShowSimplifiedGuidsAction() { return _showSimplifiedGuidsAction; }
    ToggleAction& getShowStatusBarAction() { return _showStatusBarAction; }
    OptionsAction& getStatusBarOptionsAction() { return _statusBarOptionsAction; }
    HorizontalGroupAction& getStatusBarSettingsGroupAction() { return _statusBarSettingsGroupAction; }

private:
    ToggleAction            _ignoreLoadingErrorsAction;                     /** Toggle between asking for ignoring loading errors or not */
    ToggleAction            _askConfirmationBeforeRemovingDatasetsAction;   /** Toggle between asking permission before removing datasets or not */
    ToggleAction            _keepDescendantsAfterRemovalAction;             /** Toggle between asking permission before removing datasets or not */
    ToggleAction            _showSimplifiedGuidsAction;                     /** Toggle between asking permission before removing datasets or not */
    ToggleAction            _showStatusBarAction;                           /** Action for toggling the status bar visibility */
    OptionsAction           _statusBarOptionsAction;                        /** Options action for toggling status bar items on/off */
    HorizontalGroupAction   _statusBarSettingsGroupAction;                  /** For grouping together the former two actions */
};

}
