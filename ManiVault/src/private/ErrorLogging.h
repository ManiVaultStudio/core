// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <actions/ToggleAction.h>
#include <actions/StringAction.h>

#include <QObject>
#include <QTimer>

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

private:

    /**
     * Get Crashpad handler executable name
     * @return Name of the Crashpad handler executable name
     */
    QString getCrashpadHandlerExecutableName() const;

public: // Action getters

    mv::gui::ToggleAction& getUserHasOptedAction() { return _userHasOptedAction; }
    mv::gui::ToggleAction& getEnabledAction() { return _enabledAction; }
    mv::gui::StringAction& getDsnAction() { return _dsnAction; }
    mv::gui::ToggleAction& getShowCrashReportDialogAction() { return _showCrashReportDialogAction; }

    const mv::gui::ToggleAction& getUserHasOptedAction() const { return _userHasOptedAction; }
    const mv::gui::ToggleAction& getEnabledAction() const { return _enabledAction; }
    const mv::gui::StringAction& getDsnAction() const { return _dsnAction; }
    const mv::gui::ToggleAction& getShowCrashReportDialogAction() const { return _showCrashReportDialogAction; }

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
    bool                    _initialized;                   /** Boolean determining whether the error logging has been _initialized or not */
    QTimer                  _notificationTimer;             /** Notification timer for preventing unnecessary notifications */
    mv::gui::ToggleAction   _userHasOptedAction;            /** Whether the user has opted in or out */ 
    mv::gui::ToggleAction   _enabledAction;                 /** Toggle error logging on/off */
    mv::gui::StringAction   _dsnAction;                     /** Error logging data source name action */
    mv::gui::ToggleAction   _showCrashReportDialogAction;   /** Toggle crash dialog on/off */
};
