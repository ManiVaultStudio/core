// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "GlobalSettingsGroupAction.h"

#include "actions/ToggleAction.h"
#include "actions/OptionAction.h"

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

public: // Action getters

    ToggleAction& getIgnoreLoadingErrorsAction() { return _ignoreLoadingErrorsAction; }
    ToggleAction& getAskConfirmationBeforeRemovingDatasetsAction() { return _askConfirmationBeforeRemovingDatasetsAction; }
    ToggleAction& getKeepDescendantsAfterRemovalAction() { return _keepDescendantsAfterRemovalAction; }
    ToggleAction& getShowSimplifiedGuidsAction() { return _showSimplifiedGuidsAction; }

private:
    ToggleAction   _ignoreLoadingErrorsAction;                     /** Toggle between asking for ignoring loading errors or not */
    ToggleAction   _askConfirmationBeforeRemovingDatasetsAction;   /** Toggle between asking permission before removing datasets or not */
    ToggleAction   _keepDescendantsAfterRemovalAction;             /** Toggle between asking permission before removing datasets or not */
    ToggleAction   _showSimplifiedGuidsAction;                      /** Toggle between asking permission before removing datasets or not */
};

}
