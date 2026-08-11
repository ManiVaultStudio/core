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
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    SentryErrorLogger(QObject* parent = nullptr);

    /** Connects to the error logging global settings */
    void initialize() override;

    /** Sends a caught exception as a non-fatal Sentry event. */
    void reportHandledException(const QString& title, const QString& exceptionType, const QString& reason, mv::util::SeverityLevel severity, const mv::util::StackTrace& stackTrace, const QString& diagnosticId = {}, const QString& where = {}) override;

private:

    /**
     * Get Crashpad handler executable name
     * @return Name of the Crashpad handler executable name
     */
    QString getCrashpadHandlerExecutableName() const;

    /** Start error logging to */
    void start() override;

    void stop() override;

    /**
     * Get release string for Sentry
     * @return Release string
     */
    static QString getReleaseString();

    /**
     * Get whether the Sentry data source name (DSN) is valid
     * @return Boolean determining whether the DSN is valid
     */
    bool isDsnValid() const override;

private:
    bool                    _isRunning = false;                  /** Whether the Sentry SDK initialized successfully */
    QElapsedTimer           _handledExceptionSessionTimer;      /** Monotonic clock used for handled-exception throttling */
    QHash<QString, qint64>  _handledExceptionLastSent;          /** Last transmission time for each handled-exception fingerprint */
    qsizetype               _handledExceptionsSent = 0;         /** Number of handled exceptions sent during this session */
};
