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

    /** Connects to the application */
    static void initialize();

protected:

    /**
     * Get whether the user has opted in or out
     * @return Boolean determining whether the user has opted in or out
     */
    static bool getUserHasOpted();

    /**
     * Set whether the \p userHasOpted in or out
     * @param userHasOpted Boolean determining whether the user has opted in or out
     */
    static void setUserHasOpted(bool userHasOpted);

    /**
     * Get whether error logging is enabled or not
     * @return Boolean determining whether error logging is enabled or not
     */
    static bool getErrorLoggingEnabled();

    /**
     * Set error logging enabled to \p errorLoggingEnabled
     * @param errorLoggingEnabled Boolean determining whether error logging is enabled or not
     */
    static void setErrorLoggingEnabled(bool errorLoggingEnabled);

    /**
     * Get Crashpad handler executable name
     * @return Name of the Crashpad handler executable name
     */
    static QString getCrashpadHandlerExecutableName();

private:

    /**
     * Set whether error logging is enabled or not
     * @param enabled Boolean determining whether error logging is enabled or not
     * @param force Enabled/disable, regardless whether the state has changed or not
     */
    static void setEnabled(bool enabled, bool force = false);

    /**
     * Get release string for Sentry
     * @return Release string
     */
    static QString getReleaseString();

    static bool initialized;                        /** Boolean determinging whether the error logging has been initialized or not */
    static const QString userHasOptedSettingsKey;   /** Settings key for storing whether the user has made choice the choice to opt in or out of automated error logging */
    static const QString enabledSettingsKey;        /** Settings key for storing whether the user wants error logging or not */

    friend class ErrorLoggingConsentDialog;
};
