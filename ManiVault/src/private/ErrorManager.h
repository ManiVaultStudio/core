// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <AbstractErrorManager.h>

namespace mv
{

/**
 * @brief Manages application error handling and the configured reporting backend.
 *
 * Owns the global error-reporting settings, initializes the active logger and
 * forwards handled exceptions and standalone feedback when reporting is allowed.
 *
 * @author Thomas Kroes
 */
class ErrorManager : public mv::AbstractErrorManager
{
    Q_OBJECT

public:

    /**
     * @brief Constructs the application error manager.
     * @param parent Optional parent object.
     */
    ErrorManager(QObject* parent);

    /**
     * @brief Shuts down error reporting and destroys the manager.
     */
    ~ErrorManager() override;

    /**
     * @brief Initializes settings, consent handling and the configured logger.
     */
    void initialize() override;

    /**
     * @brief Stops the configured logger and resets the manager.
     */
    void reset() override;

    /**
     * @brief Shows the error-reporting consent dialog.
     */
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

    /**
     * @brief Forwards a caught exception to the configured error logger.
     * @param title User-facing title of the exception dialog.
     * @param exceptionType Exception class or category name.
     * @param reason Technical reason supplied by the exception.
     * @param severity Severity assigned to the exception.
     * @param stackTrace Structured stack trace captured for the exception.
     * @param diagnosticId Identifier shown to the user for support correlation.
     * @param where Optional source context in which the exception was handled.
     */
    void reportHandledException(const QString& title, const QString& exceptionType, const QString& reason, util::SeverityLevel severity, const util::StackTrace& stackTrace, const QString& diagnosticId = {}, const QString& where = {}) override;

    /**
     * @brief Forwards standalone user feedback to the configured error logger.
     * @param type Feedback category, such as a problem report or feature request.
     * @param message User-provided feedback description.
     * @param email Optional email address for follow-up.
     * @return Boolean determining whether the feedback was accepted for transmission.
     */
    bool submitUserFeedback(const QString& type, const QString& message, const QString& email) override;

public: // Action getters

    gui::TriggerAction& getLoggingAskConsentDialogAction() override { return _loggingAskConsentDialogAction; }              /**< Returns the action for asking error logging consent */
    gui::ToggleAction& getLoggingUserHasOptedAction() override  { return _loggingUserHasOptedAction; }                      /**< Returns the action tracking whether the user has opted in */
    gui::ToggleAction& getLoggingEnabledAction() override { return _loggingEnabledAction; }                                 /**< Returns the action for enabling error logging */
    gui::StringAction& getLoggingDsnAction() override { return _loggingDsnAction; }                                         /**< Returns the error logging data source name action */
    gui::ToggleAction& getLoggingReportHandledExceptionsAction() override { return _loggingReportHandledExceptionsAction; } /**< Returns the action for reporting handled exceptions */
    gui::ToggleAction& getLoggingShowCrashReportDialogAction() override { return _loggingShowCrashReportDialogAction; }     /**< Returns the crash report dialog toggle action */

private:

    gui::TriggerAction  _loggingAskConsentDialogAction;         /**< Shows the consent dialog */
    gui::ToggleAction   _loggingUserHasOptedAction;             /**< Tracks whether the user has opted in or out */
    gui::ToggleAction   _loggingEnabledAction;                  /**< Toggles error logging on or off */
    gui::StringAction   _loggingDsnAction;                      /**< Error logging data source name action */
    gui::ToggleAction   _loggingReportHandledExceptionsAction;  /**< Toggles reporting of handled exceptions */
    gui::ToggleAction   _loggingShowCrashReportDialogAction;    /**< Toggles crash report dialog visibility */
};

}
