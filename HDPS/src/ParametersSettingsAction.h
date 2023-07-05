// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "GlobalSettingsGroupAction.h"

#include "actions/ToggleAction.h"

namespace hdps
{

/**
 * Parameters global settings action class
 *
 * Action class which groups all (shared) parameters based global settings
 *
 * @author Thomas Kroes
 */
class ParametersSettingsAction final : public GlobalSettingsGroupAction
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     */
    ParametersSettingsAction(QObject* parent);

public: // Action getters

    gui::ToggleAction& getAskForSharedParameterNameAction() { return _askForSharedParameterNameAction; }
    gui::ToggleAction& getConfirmRemoveSharedParameterAction() { return _confirmRemoveSharedParameterAction; }
    gui::ToggleAction& getExpertModeAction() { return _expertModeAction; }

private:
    gui::ToggleAction   _askForSharedParameterNameAction;       /** Toggle between asking for a name when publishing an action or not */
    gui::ToggleAction   _confirmRemoveSharedParameterAction;    /** Toggle asking for confirmation prior to removal of public action */
    gui::ToggleAction   _expertModeAction;                      /** In expert mode, all descendants of a root public action are displayed, otherwise they are hidden */
};

}
