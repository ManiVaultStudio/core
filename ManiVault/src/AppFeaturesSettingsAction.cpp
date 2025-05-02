// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "AppFeaturesSettingsAction.h"

namespace mv::gui
{

AppFeaturesSettingsAction::AppFeaturesSettingsAction(QObject* parent) :
    GlobalSettingsGroupAction(parent, "App Features", false)
{
//#ifdef ERROR_LOGGING
//    _groupsAction.addGroupAction(&mv::settings().getErrorLoggingSettingsAction());
//#endif
}

/*
const gui::TriggerAction& AppFeaturesSettingsAction::getLoggingAskConsentDialogAction() const
{
	return mv::errors().getLoggingAskConsentDialogAction();
}

const gui::ToggleAction& AppFeaturesSettingsAction::getUserHasOptedAction() const
{
    return mv::errors().getLoggingUserHasOptedAction();
}

const gui::ToggleAction& AppFeaturesSettingsAction::getEnabledAction() const
{
    return mv::errors().getLoggingEnabledAction();
}

const gui::StringAction& AppFeaturesSettingsAction::getDsnAction() const
{
    return mv::errors().getLoggingDsnAction();
}

const gui::ToggleAction& AppFeaturesSettingsAction::getShowCrashReportDialogAction() const
{
    return mv::errors().getLoggingShowCrashReportDialogAction();
}
*/

}
