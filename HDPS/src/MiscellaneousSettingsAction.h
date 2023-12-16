// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "GlobalSettingsGroupAction.h"

#include "actions/ToggleAction.h"

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
    gui::ToggleAction& getConfirmDatasetsRemovalAction() { return _confirmDatasetsRemovalAction; }

private:
    gui::ToggleAction   _ignoreLoadingErrorsAction;     /** Toggle action for ignoring loading errors */
    gui::ToggleAction   _confirmDatasetsRemovalAction;  /** Toggle action to determine whether to ask for permission prior to removing datasets */
};

}
