// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ApplicationSettingsAction.h"
#include "Application.h"

namespace hdps
{

ApplicationSettingsAction::ApplicationSettingsAction(QObject* parent) :
    GlobalSettingsGroupAction(parent, "Application"),
    _appearanceOptionAction(this, "Appearance", QStringList({"Light", "Dark", "System"}), "System")
{
    setShowLabels(true);
    
    _appearanceOptionAction.setDefaultWidgetFlags(gui::OptionAction::HorizontalButtons);
    _appearanceOptionAction.setSettingsPrefix(getSettingsPrefix() + "AppearanceOption");
    
    //_appearanceOptionAction.getWidget(_appearanceOptionAction, gui::OptionAction::HorizontalButtons);
    
    
    //_backgroundImage(":/Images/StartPageBackground"),
    
    addAction(&_appearanceOptionAction);

    //connect(&_expertModeAction, &ToggleAction::toggled, this, updateExpertModeActionTooltip);

    //_expertModeAction.setIcon(Application::getIconFont("FontAwesome").getIcon("user-graduate"));
}

}
