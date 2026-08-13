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

    /**
     * @brief Gets the action that opens the error-reporting consent dialog.
     * @return Consent-dialog trigger action.
     */
    const gui::TriggerAction& getLoggingAskConsentDialogAction() const;

    /**
     * @brief Gets the action that stores whether the user opted into reporting.
     * @return User consent toggle action.
     */
    const gui::ToggleAction& getUserHasOptedAction() const;

    /**
     * @brief Gets the action that enables or disables error reporting.
     * @return Error-reporting enabled toggle action.
     */
    const gui::ToggleAction& getEnabledAction() const;

    /**
     * @brief Gets the configured Sentry Data Source Name action.
     * @return Sentry DSN string action.
     */
    const gui::StringAction& getDsnAction() const;

    /**
     * @brief Gets the action controlling handled-exception reporting.
     * @return Handled-exception reporting toggle action.
     */
    const gui::ToggleAction& getReportHandledExceptionsAction() const;

    /**
     * @brief Gets the action controlling whether crash feedback is requested.
     * @return Crash-feedback dialog toggle action.
     */
    const gui::ToggleAction& getShowCrashReportDialogAction() const;
};

}
