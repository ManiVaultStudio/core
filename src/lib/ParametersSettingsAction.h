// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "GlobalSettingsGroupAction.h"

#include "actions/ToggleAction.h"

namespace mv::gui
{

/**
 * @brief Global settings for shared-parameter behavior.
 *
 * Controls shared-parameter naming, removal confirmation, and expert-mode display.
 * This settings group is classified as advanced in the Settings Manager.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ParametersSettingsAction final : public GlobalSettingsGroupAction
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     */
    ParametersSettingsAction(QObject* parent);

public: // Action getters

    ToggleAction& getAskForSharedParameterNameAction() { return _askForSharedParameterNameAction; }
    ToggleAction& getConfirmRemoveSharedParameterAction() { return _confirmRemoveSharedParameterAction; }
    ToggleAction& getExpertModeAction() { return _expertModeAction; }

private:
    ToggleAction   _askForSharedParameterNameAction;       /** Toggle between asking for a name when publishing an action or not */
    ToggleAction   _confirmRemoveSharedParameterAction;    /** Toggle asking for confirmation prior to removal of public action */
    ToggleAction   _expertModeAction;                      /** In expert mode, all descendants of a root public action are displayed, otherwise they are hidden */
};

}
