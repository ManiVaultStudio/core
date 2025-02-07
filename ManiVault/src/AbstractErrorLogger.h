// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "Application.h"

#include "actions/ToggleAction.h"
#include "actions/StringAction.h"

#include <QObject>
#include <QTimer>

#include <queue>

namespace mv
{

/**
 * Abstract error logger class
 *
 * Base class for logging errors.
 *
 * @author Thomas Kroes
 */
class AbstractErrorLogger : public QObject
{
protected:

    /** Notification struct */
    struct Notification {
        QString         _title;         /**< Title of the notification */
        QString         _message;       /**< Message of the notification */
        QIcon           _icon;          /**< Icon of the notification */
        std::int32_t    _delay = 0;     /**< Delay of the notification */
    };

    using Notifications = std::map<QString, Notification>;

public:

    /**
     * Construct with pointer to \p parent object
     * @param loggerName Name of the logger (e.g. Sentry)
     * @param parent Pointer to parent object
     */
    AbstractErrorLogger(const QString& loggerName, QObject* parent = nullptr) :
        QObject(parent),
        _loggerName(loggerName),
        _initialized(false)
    {
        _notificationTimer.setInterval(2500);
        
        connect(&_notificationTimer, &QTimer::timeout, this, [this]() -> void {
            for (auto& pair : _notifications) {
                const auto& notification = pair.second;

                mv::help().addNotification(notification._title, notification._message, notification._icon, util::Notification::DurationType::Calculated, notification._delay);

				_notifications.erase(pair.first);
            }
		});

        _notificationTimer.start();
    }

    /** Connects to the error logging global settings */
    virtual void initialize() = 0;

    /**
     * Get the logger name
     * @return Logger name
     */
    QString getLoggerName() const {
	    return _loggerName;
    }

    /** Starts the error logger if the pre-flight conditions are met */
    void requestStart()
    {
        if (getUserHasOptedAction().isChecked() && getEnabledAction().isChecked()) {
            start();
        }
    }

    /**
     * Get whether the error logger is initialized or not
     * @return Boolean determining whether the error logger is initialized or not
     */
    bool isInitialized() const {
	    return _initialized;
    }

    /**
     * Add /p notification
     * @param name Name of the notification
     * @param notification Notification
     */
    void addNotification(const QString& name, const Notification& notification) {
        _notifications[name] = notification;
    }

    /** Begin the initialization of the error logger */
    virtual void beginInitialization() {
        connect(&mv::errors().getLoggingEnabledAction(), &gui::ToggleAction::toggled, this, [this](bool toggled) -> void {
            addNotification("Enabled", {
                QString("%1 error logging").arg(_loggerName),
                QString("Logging will be <b>%1</b> after restarting the application.").arg(toggled ? "enabled" : "disabled"),
                Application::getIconFont("FontAwesome").getIcon("bug")
            });
        });

        connect(&mv::errors().getLoggingDsnAction(), &gui::StringAction::stringChanged, this, [this](const QString& dsn) -> void {
            if (mv::errors().getLoggingDsnAction().isValid() == QValidator::Acceptable)
                addNotification("DSN", {
                	QString("%1 error logging").arg(_loggerName),
                	QString("The logging Data Source Name (DSN) will be changed to <a href='%1'>%1</a> after restarting the application.").arg(dsn),
                	Application::getIconFont("FontAwesome").getIcon("bug")
                });
		});

        connect(&mv::errors().getLoggingShowCrashReportDialogAction(), &gui::ToggleAction::toggled, this, [this](bool toggled) -> void {
            if (toggled)
                addNotification("ShowCrashReportDialog", {
                        QString("%1 error logging").arg(_loggerName),
                        QString("A crash report dialog will popup when an fatal error occurs."),
                        Application::getIconFont("FontAwesome").getIcon("bug")
				});
            else
                addNotification("ShowCrashReportDialog", {
                    QString("%1 error logging").arg(_loggerName),
                    QString("A crash report will be sent to the Sentry server unsupervised."),
                    Application::getIconFont("FontAwesome").getIcon("bug")
                });
		});
    }

    /** End the initialization of the error logger */
    virtual void endInitialization() {
        _initialized = true;
    }

private:

    /** Starts the error logger */
    virtual void start() = 0;

    /**
     * Get whether the Sentry data source name (DSN) is valid
     * @return Boolean determining whether the DSN is valid
     */
    virtual bool isDsnValid() const = 0;

protected: // Action getters

    static gui::TriggerAction& getAskConsentDialogAction() { return mv::errors().getLoggingAskConsentDialogAction(); }              /** Get action for asking the user for consent to log errors */
    static gui::ToggleAction& getUserHasOptedAction() { return mv::errors().getLoggingUserHasOptedAction(); }                       /** Get action for user has opted */
    static gui::ToggleAction& getEnabledAction() { return mv::errors().getLoggingEnabledAction(); }                                 /** Get action for logging enabled */
    static gui::StringAction& getDsnAction() { return mv::errors().getLoggingDsnAction(); };                                        /** Get action for logging data source name (DSN) */
    static gui::ToggleAction& getShowCrashReportDialogAction() { return mv::errors().getLoggingShowCrashReportDialogAction(); }     /** Get action for showing a crash report dialog when the application fails */

private:

    /** Set settings dirty (notifies users that a restart is required) */
    void setSettingsDirty() {
        _notificationTimer.start();
    }

private:
    const QString   _loggerName;            /** Name of the logger */
    bool            _initialized;           /** Boolean determining whether the error logger has been initialized or not */
    QTimer          _notificationTimer;     /** Notification timer for preventing unnecessary notifications */
    Notifications   _notifications;         /** Notifications */
};

}
