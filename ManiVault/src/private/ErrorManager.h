// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <AbstractErrorManager.h>

namespace mv
{

class ErrorManager : public mv::AbstractErrorManager
{
    Q_OBJECT

public:

    /**
     * Construct manager with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    ErrorManager(QObject* parent);

    /** Reset when destructed */
    ~ErrorManager() override;

    /** Perform event manager startup initialization */
    void initialize() override;

    /** Resets the contents of the event manager */
    void reset() override;

    /** Show the error logging consent dialog */
    void showErrorLoggingConsentDialog() override;

public: // Action getters

    gui::TriggerAction& getLoggingAskConsentDialogAction() override { return _loggingAskConsentDialogAction; }              /** Get action for asking the user for consent to log errors */
    gui::ToggleAction& getLoggingUserHasOptedAction() override  { return _loggingUserHasOptedAction; }                      /** Get action for user has opted */
    gui::ToggleAction& getLoggingEnabledAction() override { return _loggingEnabledAction; }                                 /** Get action for logging enabled */
    gui::StringAction& getLoggingDsnAction() override { return _loggingDsnAction; }                                         /** Get action for logging data source name (DSN) */
    gui::ToggleAction& getLoggingShowCrashReportDialogAction() override { return _loggingShowCrashReportDialogAction; }     /** Get action for showing a crash report dialog when the application fails */

private:
    gui::TriggerAction  _loggingAskConsentDialogAction;         /** Show the consent dialog */
    gui::ToggleAction   _loggingUserHasOptedAction;             /** Whether the user has opted in or out */
    gui::ToggleAction   _loggingEnabledAction;                  /** Toggle error logging on/off */
    gui::StringAction   _loggingDsnAction;                      /** Error logging data source name action */
    gui::ToggleAction   _loggingShowCrashReportDialogAction;    /** Toggle crash dialog on/off */
};

}
