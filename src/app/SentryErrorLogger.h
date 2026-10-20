// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <AbstractErrorLogger.h>

#include <QElapsedTimer>
#include <QHash>

/**
 * Sentry error logger class
 *
 * For logging errors with Sentry.
 *
 * @author Thomas Kroes
 */
class SentryErrorLogger : public mv::AbstractErrorLogger
{
public:

    /**
     * @brief Constructs a Sentry error logger.
     * @param parent Pointer to the parent object (maybe nullptr).
     */
    SentryErrorLogger(QObject* parent = nullptr);

    /**
     * @brief Initializes the Sentry logger and connects it to the global settings.
     */
    void initialize() override;

    /**
     * @brief Sends a caught exception as a non-fatal Sentry event.
     * @param title User-facing title of the exception dialog.
     * @param exceptionType Exception class or category name.
     * @param reason Technical reason supplied by the exception.
     * @param severity Severity assigned to the exception.
     * @param stackTrace Structured stack trace captured for the exception.
     * @param diagnosticId Identifier shown to the user for support correlation.
     * @param where Optional source context in which the exception was handled.
     */
    void reportHandledException(const QString& title, const QString& exceptionType, const QString& reason, mv::util::SeverityLevel severity, const mv::util::StackTrace& stackTrace, const QString& diagnosticId = {}, const QString& where = {}) override;

    /**
     * @brief Sends a standalone item through Sentry's modern User Feedback API.
     * @param type Feedback category, such as a problem report or feature request.
     * @param message User-provided feedback description.
     * @param email Optional email address for follow-up.
     * @return Boolean determining whether Sentry accepted the feedback for transmission.
     */
    bool submitUserFeedback(const QString& type, const QString& message, const QString& email) override;

private:

    /**
     * @brief Gets the platform-specific Crashpad handler executable name.
     * @return Crashpad handler executable name, or an empty string when unsupported.
     */
    QString getCrashpadHandlerExecutableName() const;

    /**
     * @brief Initializes the Sentry SDK and starts error reporting.
     */
    void start() override;

    /**
     * @brief Closes the Sentry SDK and preserves envelopes that could not be flushed.
     */
    void stop() override;

    /**
     * @brief Gets the release identifier supplied to Sentry.
     * @return Sentry release identifier for the current ManiVault version.
     */
    static QString getReleaseString();

    /**
     * @brief Determines whether the configured Sentry Data Source Name is valid.
     * @return Boolean determining whether the DSN is structurally valid.
     */
    bool isDsnValid() const override;

private:
    bool                    _isRunning = false;                  /** Whether the Sentry SDK initialized successfully */
    QElapsedTimer           _handledExceptionSessionTimer;      /** Monotonic clock used for handled-exception throttling */
    QHash<QString, qint64>  _handledExceptionLastSent;          /** Last transmission time for each handled-exception fingerprint */
    qsizetype               _handledExceptionsSent = 0;         /** Number of handled exceptions sent during this session */
};
