// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/ToggleAction.h"
#include "actions/StringAction.h"
#include "actions/TriggerAction.h"

#include "CoreInterface.h"

#include "util/StyledIcon.h"
#include "util/SeverityLevel.h"
#include "util/StackFrame.h"

#include <QTimer>

namespace mv
{

/**
 * Abstract error logger class
 *
 * Base class for logging errors.
 *
 * @ingroup mv_managers
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
        _initialized(false),
        _notificationTimer(),
        _notifications()
    {
        _notificationTimer.setInterval(2500);

        connect(&_notificationTimer, &QTimer::timeout, this, [this]() -> void {
            for (auto& [name, notification] : _notifications) {
                mv::help().addNotification(notification._title, notification._message, notification._icon, util::Notification::DurationType::Calculated, notification._delay);
            }
            _notifications.clear();
            });

        _notificationTimer.start();
    }

    /**
     * @brief Initializes the error logger and connects it to the global error-reporting settings.
     */
    virtual void initialize() = 0;

    /**
     * @brief Reports an exception that was caught and presented to the user.
     * @param title User-facing title of the exception dialog.
     * @param exceptionType Exception class or category name.
     * @param reason Technical reason supplied by the exception.
     * @param severity Severity assigned to the exception.
     * @param stackTrace Structured stack trace captured for the exception.
     * @param diagnosticId Identifier shown to the user for support correlation.
     * @param where Optional source context in which the exception was handled.
     */
    virtual void reportHandledException(const QString& title, const QString& exceptionType, const QString& reason, util::SeverityLevel severity, const util::StackTrace& stackTrace, const QString& diagnosticId = {}, const QString& where = {})
    {
        Q_UNUSED(title)
        Q_UNUSED(exceptionType)
        Q_UNUSED(reason)
        Q_UNUSED(severity)
        Q_UNUSED(stackTrace)
        Q_UNUSED(diagnosticId)
        Q_UNUSED(where)
    }

    /**
     * @brief Sends standalone user feedback when the logger is available.
     * @param type Feedback category, such as a problem report or feature request.
     * @param message User-provided feedback description.
     * @param email Optional email address for follow-up.
     * @return Boolean determining whether the feedback was accepted for transmission.
     */
    virtual bool submitUserFeedback(const QString& type, const QString& message, const QString& email)
    {
        Q_UNUSED(type)
        Q_UNUSED(message)
        Q_UNUSED(email)
        return false;
    }

    /**
     * Get the logger name
     * @return Logger name
     */
    QString getLoggerName() const {
        return _loggerName;
    }

    /**
     * @brief Starts the error logger when the user has opted in and reporting is enabled.
     */
    void requestStart()
    {
        if (getUserHasOptedAction().isChecked() && getEnabledAction().isChecked()) {
            start();
        }
    }

    /**
     * @brief Stops the error logger and completes its shutdown procedure.
     */
    void requestStop()
    {
        stop();
    }

    /**
     * Get whether the error logger is initialized or not
     * @return Boolean determining whether the error logger is initialized or not
     */
    bool isInitialized() const {
        return _initialized;
    }

    /**
     * @brief Adds or replaces a delayed user notification.
     * @param name Name of the notification
     * @param notification Notification
     */
    void addNotification(const QString& name, const Notification& notification) {
        _notifications[name] = notification;
    }

    /**
     * @brief Begins initialization and connects common error-reporting setting notifications.
     */
    virtual void beginInitialization() {
        if (!mv::errors().getLoggingUserHasOptedAction().isChecked())
            return;

        connect(&mv::errors().getLoggingEnabledAction(), &gui::ToggleAction::toggled, this, [this](bool toggled) -> void {
            addNotification("Enabled", {
                QString("%1 error logging").arg(_loggerName),
                QString("Logging will be <b>%1</b> after restarting the application.").arg(toggled ? "enabled" : "disabled"),
                util::StyledIcon("bug")
            });
        });

        connect(&mv::errors().getLoggingDsnAction(), &gui::StringAction::stringChanged, this, [this](const QString& dsn) -> void {
            if (mv::errors().getLoggingDsnAction().isValid() == QValidator::Acceptable)
                addNotification("DSN", {
                    QString("%1 error logging").arg(_loggerName),
                    QString("The logging Data Source Name (DSN) will be changed to <a href='%1'>%1</a> after restarting the application.").arg(dsn),
                    util::StyledIcon("bug")
                });
        });

        connect(&mv::errors().getLoggingShowCrashReportDialogAction(), &gui::ToggleAction::toggled, this, [this](bool toggled) -> void {
            if (toggled)
                addNotification("ShowCrashReportDialog", {
                    QString("%1 error logging").arg(_loggerName),
                    QString("A feedback window will open on the next launch after a crash. This setting will take effect after restarting the application."),
                    util::StyledIcon("bug")
                });
            else
                addNotification("ShowCrashReportDialog", {
                    QString("%1 error logging").arg(_loggerName),
                    QString("You will not be asked for optional feedback after a crash. Technical crash reporting remains controlled by the error logging setting. This setting will take effect after restarting the application."),
                    util::StyledIcon("bug")
                });
        });
    }

    /**
     * @brief Completes initialization and marks the logger as initialized.
     */
    virtual void endInitialization() {
        _initialized = true;
    }

private:

    /**
     * @brief Starts the concrete error-logging backend.
     */
    virtual void start() = 0;

    /**
     * @brief Shuts down the concrete error-logging backend.
     */
    virtual void stop() = 0;

    /**
     * Get whether the Sentry data source name (DSN) is valid
     * @return Boolean determining whether the DSN is valid
     */
    virtual bool isDsnValid() const = 0;

protected: // Action getters

    static gui::TriggerAction& getAskConsentDialogAction() { return mv::errors().getLoggingAskConsentDialogAction(); }              /** Get action for asking the user for consent to log errors */
    static gui::ToggleAction& getUserHasOptedAction() { return mv::errors().getLoggingUserHasOptedAction(); }                       /** Get action for user has opted */
    static gui::ToggleAction& getEnabledAction() { return mv::errors().getLoggingEnabledAction(); }                                 /** Get action for logging enabled */
    static gui::StringAction& getDsnAction() { return mv::errors().getLoggingDsnAction(); }                                         /** Get action for logging data source name (DSN) */
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
