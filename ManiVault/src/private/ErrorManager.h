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
     * @brief Constructs the application error manager.
     * @param parent Optional parent object.
     */
    ErrorManager(QObject* parent);

    /** Resets the manager when destructed. */
    ~ErrorManager() override;

    /** Performs error manager startup initialization. */
    void initialize() override;

    /** Resets the contents of the error manager. */
    void reset() override;

    /** Shows the error logging consent dialog. */
    void showErrorLoggingConsentDialog() override;

    /**
     * @brief Gets a formatted stack trace for debugging purposes.
     *
     * Returns a human-readable stack trace of the current execution point. The
     * default implementation returns an empty string, indicating that stack trace
     * generation is not supported or has not been implemented.
     *
     * Derived classes may override this function to provide platform- or
     * application-specific stack trace generation using the available debugging
     * facilities.
     *
     * @return A formatted stack trace, or an empty string if unavailable.
     */
    QString getFormattedDebugStackTrace() const override;


    /**
     * @brief Gets a structured stack trace for debugging purposes.
     *
     * Returns stack frames describing the current execution point. The default
     * implementation returns an empty stack trace, indicating that stack trace
     * capture is not supported or has not been implemented by the active error
     * manager.
     *
     * Derived classes may override this function to provide platform- or
     * application-specific stack trace collection.
     *
     * @return Stack trace frames, or an empty stack trace if unavailable.
     */
    util::StackTrace getDebugStackTrace() const override;

public: // Action getters

    gui::TriggerAction& getLoggingAskConsentDialogAction() override { return _loggingAskConsentDialogAction; }              /**< Returns the action for asking error logging consent */
    gui::ToggleAction& getLoggingUserHasOptedAction() override  { return _loggingUserHasOptedAction; }                      /**< Returns the action tracking whether the user has opted in */
    gui::ToggleAction& getLoggingEnabledAction() override { return _loggingEnabledAction; }                                 /**< Returns the action for enabling error logging */
    gui::StringAction& getLoggingDsnAction() override { return _loggingDsnAction; }                                         /**< Returns the error logging data source name action */
    gui::ToggleAction& getLoggingShowCrashReportDialogAction() override { return _loggingShowCrashReportDialogAction; }     /**< Returns the crash report dialog toggle action */

private:

    gui::TriggerAction  _loggingAskConsentDialogAction;         /**< Shows the consent dialog */
    gui::ToggleAction   _loggingUserHasOptedAction;             /**< Tracks whether the user has opted in or out */
    gui::ToggleAction   _loggingEnabledAction;                  /**< Toggles error logging on or off */
    gui::StringAction   _loggingDsnAction;                      /**< Error logging data source name action */
    gui::ToggleAction   _loggingShowCrashReportDialogAction;    /**< Toggles crash report dialog visibility */
};

}
