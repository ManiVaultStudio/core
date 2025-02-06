// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QObject>

/**
 * Error logging class
 *
 * For managing error logging with Sentry.
 *
 * @author Thomas Kroes
 */
class ErrorLogging : public QObject
{
public:

    /**
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    ErrorLogging(QObject* parent = nullptr);

    /** Connects to the error logging global settings */
    void initialize();

protected:

    /**
     * Get whether the user has opted in or out
     * @return Boolean determining whether the user has opted in or out
     */
    bool getUserHasOpted() const;

    /**
     * Set whether the \p userHasOpted in or out
     * @param userHasOpted Boolean determining whether the user has opted in or out
     */
    void setUserHasOpted(bool userHasOpted);

    /**
     * Get whether error logging is enabled or not
     * @return Boolean determining whether error logging is enabled or not
     */
    bool getErrorLoggingEnabled() const;

    /**
     * Set error logging enabled to \p errorLoggingEnabled
     * @param errorLoggingEnabled Boolean determining whether error logging is enabled or not
     */
    void setErrorLoggingEnabled(bool errorLoggingEnabled);

    /**
     * Get error logging Data Source Name (DSN)
     * @return Error logging DSN
     */
    QString getErrorLoggingDsn() const;

    /**
     * Set error logging Data Source Name (DSN) to \p errorLoggingDsn
     * @param errorLoggingDsn Error logging DSN
     */
    void setErrorLoggingDsn(const QString& errorLoggingDsn);

    /**
     * Get Crashpad handler executable name
     * @return Name of the Crashpad handler executable name
     */
    QString getCrashpadHandlerExecutableName() const;

private:

    /** Start error logging to */
    void start();

    /**
     * Get release string for Sentry
     * @return Release string
     */
    QString getReleaseString();

    /** Set settings dirty (requires restart) */
    void setSettingsDirty();

    /**
     * Get whether the Sentry data source name (DSN) is valid
     * @return Boolean determining whether the DSN is valid
     */
    bool isDsnValid() const;

private:
    bool _initialized;       /** Boolean determining whether the error logging has been _initialized or not */

    static const QString userHasOptedSettingsKey;   /** Settings key for storing whether the user has made choice the choice to opt in or out of automated error logging */
    static const QString enabledSettingsKey;        /** Settings key for storing whether the user wants error logging or not */
    static const QString dsnSettingsKey;            /** Settings key for storing the Data Source Name (DSN) for Sentry */

    friend class ErrorLoggingConsentDialog;
};
