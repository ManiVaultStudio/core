// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ParametersSettingsAction.h"
#include "Application.h"

using namespace mv::gui;

namespace mv::gui
{

ParametersSettingsAction::ParametersSettingsAction(QObject* parent) :
    GlobalSettingsGroupAction(parent, "Parameters"),
    _askForSharedParameterNameAction(this, "Ask for shared parameter name", true),
    _confirmRemoveSharedParameterAction(this, "Confirm remove shared parameter", true),
    _expertModeAction(this, "View in expert mode", false)
{
    setShowLabels(false);

    addAction(&_askForSharedParameterNameAction);
    addAction(&_confirmRemoveSharedParameterAction);
    addAction(&_expertModeAction);

    const auto updateExpertModeActionTooltip = [this]() -> void {
        if (_expertModeAction.isChecked())
            _expertModeAction.setToolTip("Display all parameter connections (expert mode)");
        else
            _expertModeAction.setToolTip("Display simplified parameter connections");
    };

    updateExpertModeActionTooltip();

    connect(&_expertModeAction, &ToggleAction::toggled, this, updateExpertModeActionTooltip);

    _expertModeAction.setIconByName("user-graduate");
}

}
