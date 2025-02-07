// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ErrorLoggingSettingsAction.h"

namespace mv::gui
{

ErrorLoggingSettingsAction::ErrorLoggingSettingsAction(QObject* parent) :
    GlobalSettingsGroupAction(parent, "Error logging")
{
}

const gui::TriggerAction& ErrorLoggingSettingsAction::getLoggingAskConsentDialogAction() const
{
	return mv::errors().getLoggingAskConsentDialogAction();
}

const gui::ToggleAction& ErrorLoggingSettingsAction::getUserHasOptedAction() const
{
    return mv::errors().getLoggingUserHasOptedAction();
}

const gui::ToggleAction& ErrorLoggingSettingsAction::getEnabledAction() const
{
    return mv::errors().getLoggingEnabledAction();
}

const gui::StringAction& ErrorLoggingSettingsAction::getDsnAction() const
{
    return mv::errors().getLoggingDsnAction();
}

const gui::ToggleAction& ErrorLoggingSettingsAction::getShowCrashReportDialogAction() const
{
    return mv::errors().getLoggingShowCrashReportDialogAction();
}

}
