// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <AbstractErrorLogger.h>

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

    /** Begin the initialization of the error logger */
    void beginInitialization() override;

    /** Connects to the error logging global settings */
    void initialize() override;

private:

    /**
     * Get Crashpad handler executable name
     * @return Name of the Crashpad handler executable name
     */
    QString getCrashpadHandlerExecutableName() const;

    /** Start error logging to */
    void start() override;

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
};
