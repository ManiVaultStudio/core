// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ParametersSettingsAction.h"
#include "Application.h"

using namespace hdps::gui;

namespace hdps
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

    _askForSharedParameterNameAction.setSettingsPrefix(getSettingsPrefix() + "AskForSharedParameterName");
    _confirmRemoveSharedParameterAction.setSettingsPrefix(getSettingsPrefix() + "ConfirmRemoveSharedParameter");
    _expertModeAction.setSettingsPrefix(getSettingsPrefix() + "ExpertMode");

    const auto updateExpertModeActionTooltip = [this]() -> void {
        if (_expertModeAction.isChecked())
            _expertModeAction.setToolTip("Display all parameter connections (expert mode)");
        else
            _expertModeAction.setToolTip("Display simplified parameter connections");
    };

    updateExpertModeActionTooltip();

    connect(&_expertModeAction, &ToggleAction::toggled, this, updateExpertModeActionTooltip);

    _expertModeAction.setIcon(Application::getIconFont("FontAwesome").getIcon("user-graduate"));
}

}
