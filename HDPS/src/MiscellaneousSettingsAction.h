// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "GlobalSettingsGroupAction.h"

#include "actions/ToggleAction.h"
#include "actions/OptionAction.h"

namespace mv
{

/**
 * Miscellaneous global settings action class
 *
 * Action class which groups all miscellaneous global settings
 *
 * @author Thomas Kroes
 */
class MiscellaneousSettingsAction final : public GlobalSettingsGroupAction
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     */
    MiscellaneousSettingsAction(QObject* parent);

public: // Action getters

    gui::ToggleAction& getIgnoreLoadingErrorsAction() { return _ignoreLoadingErrorsAction; }
    gui::ToggleAction& getAskConfirmationBeforeRemovingDatasetsAction() { return _askConfirmationBeforeRemovingDatasetsAction; }
    gui::ToggleAction& getKeepDescendantsAfterRemovalAction() { return _keepDescendantsAfterRemovalAction; }

private:
    gui::ToggleAction   _ignoreLoadingErrorsAction;                     /** Toggle between asking for ignoring loading errors or not */
    gui::ToggleAction   _askConfirmationBeforeRemovingDatasetsAction;   /** Toggle between asking permission before removing datasets or not */
    gui::ToggleAction   _keepDescendantsAfterRemovalAction;             /** Toggle between asking permission before removing datasets or not */
};

}
