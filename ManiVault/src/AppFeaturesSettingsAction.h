// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "GlobalSettingsGroupAction.h"

#include "actions/ToggleAction.h"
#include "actions/StringAction.h"
#include "actions/TriggerAction.h"

namespace mv::gui
{

/**
 * Error logging settings action class
 *
 * Groups all error logging settings for the application
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ErrorLoggingSettingsAction final : public GlobalSettingsGroupAction
{
public:

    /**
     * Construct the error logging settings action with a parent object
     * @param parent Pointer to parent object
     */
    ErrorLoggingSettingsAction(QObject* parent);

public: // Action getters

    const gui::TriggerAction& getLoggingAskConsentDialogAction() const;     /** Get action for asking the user for consent to log errors */
    const gui::ToggleAction& getUserHasOptedAction() const;                 /** Get action for user has opted */  
    const gui::ToggleAction& getEnabledAction() const;                      /** Get action for logging enabled */
    const gui::StringAction& getDsnAction() const;                          /** Get action for logging data source name (DSN) */
    const gui::ToggleAction& getShowCrashReportDialogAction() const;        /** Get action for showing a crash report dialog when the application fails */
};

}
