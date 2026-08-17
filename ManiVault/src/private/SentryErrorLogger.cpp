// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SentryErrorLogger.h"

#include "sentry.h"

#include <QOperatingSystemVersion>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QUuid>

using namespace mv;
using namespace mv::gui;
using namespace mv::util;

namespace
{
    constexpr qsizetype maximumSentryTextLength = 4096;
    constexpr qsizetype maximumHandledExceptionsPerSession = 50;
    constexpr qint64 handledExceptionCooldownMilliseconds = 60 * 1000;
    constexpr uint64_t sentryShutdownTimeoutMilliseconds = 2000;
    constexpr auto installationIdSettingsKey          = "ErrorReporting/AnonymousInstallationId";

    /**
     * @brief Gets or creates the random identifier used for anonymous installation counts.
     * @return Stable, lower-case UUID stored in the active application's persistent settings.
     */
    QString getAnonymousInstallationId()
    {
        const auto application = Application::current();

        if (application == nullptr)
            return {};

        const auto storedInstallationId = application->getSetting(installationIdSettingsKey).toString();
        const QUuid storedUuid(storedInstallationId);

        if (!storedUuid.isNull() && storedUuid.toString(QUuid::WithoutBraces) == storedInstallationId)
            return storedInstallationId;

        const auto installationId = QUuid::createUuid().toString(QUuid::WithoutBraces);

        application->setSetting(installationIdSettingsKey, installationId);

        if (application->getSetting(installationIdSettingsKey).toString() != installationId)
            return {};

        return installationId;
    }

    QString sanitizeForSentry(const QString& text)
    {
        auto sanitized = text;

        const auto redactDirectory = [&sanitized](const QString& directory, const QString& replacement) {
            if (directory.isEmpty() || QDir(directory).isRoot())
                return;

            sanitized.replace(QDir::cleanPath(directory), replacement, Qt::CaseInsensitive);
            sanitized.replace(QDir::toNativeSeparators(QDir::cleanPath(directory)), replacement, Qt::CaseInsensitive);
        };

        redactDirectory(QDir::tempPath(), "<temp>");
        redactDirectory(QDir::homePath(), "<home>");

        static const QRegularExpression windowsUserPath(R"(([A-Z]:[\\/](?:Users|Documents and Settings)[\\/])[^\\/\s]+)", QRegularExpression::CaseInsensitiveOption);
        static const QRegularExpression unixUserPath(R"((/(?:home|Users)/)[^/\s]+)", QRegularExpression::CaseInsensitiveOption);
        static const QRegularExpression emailAddress(R"(\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\.[A-Z]{2,}\b)", QRegularExpression::CaseInsensitiveOption);
        static const QRegularExpression urlCredentials(R"((\b[A-Z][A-Z0-9+.-]*://)[^/\s:@]+(?::[^/\s@]*)?@)", QRegularExpression::CaseInsensitiveOption);

        sanitized.replace(windowsUserPath, "\\1<user>");
        sanitized.replace(unixUserPath, "\\1<user>");
        sanitized.replace(urlCredentials, "\\1<credentials>@");
        sanitized.replace(emailAddress, "<email>");

        if (sanitized.size() > maximumSentryTextLength)
            sanitized = sanitized.left(maximumSentryTextLength) + "...[truncated]";

        return sanitized;
    }
}

SentryErrorLogger::SentryErrorLogger(QObject* parent /*= nullptr*/) :
    AbstractErrorLogger("Sentry", parent)
{
}

QString SentryErrorLogger::getCrashpadHandlerExecutableName() const
{
#ifdef Q_OS_LINUX
    qDebug() << "crashpad_handler";
#endif

    switch (QOperatingSystemVersion::current().type())
    {
        case QOperatingSystemVersion::Windows:
            return "crashpad_handler.exe";

        case QOperatingSystemVersion::MacOS:
            return "crashpad_handler";

        case QOperatingSystemVersion::Unknown:
        case QOperatingSystemVersion::IOS:
        case QOperatingSystemVersion::TvOS:
        case QOperatingSystemVersion::WatchOS:
        case QOperatingSystemVersion::Android:
            return {};
    }

    return {};
}

void SentryErrorLogger::initialize()
{
    if (isInitialized())
        return;

    // Create the identifier on the first launch of a Sentry-enabled build,
    // independently of whether the user chooses to enable transmission.
    (void)getAnonymousInstallationId();

    beginInitialization();
    {
    }
    endInitialization();
}

void SentryErrorLogger::start()
{
    if (_isRunning) {
        qDebug() << "Sentry error logging is already running";
        return;
    }

    if (!getUserHasOptedAction().isChecked() || !getEnabledAction().isChecked()) {
        return;
    }

    qDebug() << "Starting Sentry error logging...";

    if (!isDsnValid()) {
        qWarning() << "Cannot start Sentry: error logging is enabled, but the DSN is empty or invalid";
        return;
    }

    const auto dsn = getDsnAction().getString();

    sentry_options_t* options = sentry_options_new();

    const auto cachePath          = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    const auto sentryDatabasePath = QDir(cachePath).filePath("Sentry");

    if (cachePath.isEmpty() || !QDir().mkpath(sentryDatabasePath)) {
        qWarning() << "Cannot start Sentry: unable to create its cache directory at" << sentryDatabasePath;

        const auto reason = cachePath.isEmpty()
            ? QString("The operating system did not provide a writable application cache location.")
            : QString("The cache directory <code>%1</code> could not be created. Check the directory permissions and available disk space.")
                  .arg(sentryDatabasePath.toHtmlEscaped());

        addNotification("CacheDirectoryUnavailable", {
            "Sentry error logging disabled",
            QString("Sentry could not start. %1 Error and crash reports will not be sent during this session.").arg(reason),
            StyledIcon("triangle-exclamation"),
            5000
        });

        sentry_options_free(options);
        return;
    }
    
    sentry_options_set_dsn(options, dsn.toUtf8());

    const auto crashpadHandlerPath = QDir(QCoreApplication::applicationDirPath()).filePath(getCrashpadHandlerExecutableName());

#ifdef Q_OS_WIN
    sentry_options_set_handler_pathw(options, reinterpret_cast<const wchar_t*>(crashpadHandlerPath.utf16()));
#else
    sentry_options_set_handler_path(options, crashpadHandlerPath.toUtf8());
#endif

    sentry_options_set_database_path(options, sentryDatabasePath.toUtf8());
    sentry_options_set_shutdown_timeout(options, sentryShutdownTimeoutMilliseconds);
    // sentry-native starts its automatic session from sentry_init(), before an
    // application-defined user can be installed. Start it explicitly after
    // setting the anonymous installation ID so Release Health receives a
    // stable distinct user without collecting machine-derived identifiers.
    sentry_options_set_auto_session_tracking(options, 0);

    if (getShowCrashReportDialogAction().isChecked()) {
#ifdef Q_OS_WIN
        const auto crashReporterExecutable = "ManiVault Crash Reporter.exe";
#else
        const auto crashReporterExecutable = "ManiVault Crash Reporter";
#endif
        const auto crashReporterPath = QDir(QCoreApplication::applicationDirPath()).filePath(crashReporterExecutable);
        const auto crashReporterInfo = QFileInfo(crashReporterPath);

        if (crashReporterInfo.exists() && crashReporterInfo.isFile()) {
#ifdef Q_OS_WIN
            sentry_options_set_external_crash_reporter_pathw(options, reinterpret_cast<const wchar_t*>(crashReporterPath.utf16()));
#else
            sentry_options_set_external_crash_reporter_path(options, crashReporterPath.toUtf8());
#endif
        } else {
            qWarning() << "Cannot enable Sentry crash feedback: the external crash reporter was not found at" << crashReporterPath
                       << "Technical crash reports will still be sent automatically.";

            addNotification("CrashReporterUnavailable", {
                "Crash feedback unavailable",
                QString("The crash feedback application could not be found at <code>%1</code>. Technical crash reports will still be sent automatically, but you will not be asked for optional feedback after a crash.")
                    .arg(crashReporterPath.toHtmlEscaped()),
                StyledIcon("triangle-exclamation"),
                5000
            });
        }
    }

    const auto releaseString = getReleaseString().toUtf8();

#ifdef _DEBUG
    sentry_options_set_debug(options, 1);
    sentry_options_set_environment(options, "debug");
    sentry_options_set_release(options, releaseString + "-debug");
#else
    sentry_options_set_debug(options, 0);
    sentry_options_set_environment(options, "release");
    sentry_options_set_release(options, releaseString + "-release");
#endif

    if (sentry_init(options) != 0) {
        qWarning() << "Cannot start Sentry: SDK initialization failed";

        addNotification("InitializationFailed", {
            "Sentry error logging disabled",
            "Sentry could not initialize. Error and crash reports will not be sent during this session. See the application log for technical details.",
            StyledIcon("triangle-exclamation"),
            5000
        });

        return;
    }

    _isRunning = true;
    _handledExceptionSessionTimer.start();
    _handledExceptionLastSent.clear();
    _handledExceptionsSent = 0;

    const auto anonymousInstallationId = getAnonymousInstallationId().toUtf8();

    if (anonymousInstallationId.isEmpty()) {
        qWarning() << "Sentry Release Health disabled: the anonymous installation ID could not be persisted";
    } else {
        sentry_set_user(sentry_value_new_user(anonymousInstallationId.constData(), nullptr, nullptr, nullptr));
        sentry_start_session();
    }

    qDebug() << "Sentry error logging is running, crash reports will send to: " + dsn;

#ifdef _DEBUG
    sentry_set_tag("build_type", "debug");
#else
    sentry_set_tag("build_type", "release");
#endif

    // The external crash reporter handles feedback and owns the crash envelope.
    // Clear the marker so a handled crash does not prompt again on later runs.
    sentry_clear_crashed_last_run();
    
    addNotification("Started", {
        QString("%1 error logging").arg(getLoggerName()),
        "Error logging using <a href='https://sentry.io/'>Sentry</a> is active, crash reports will be logged to improve the application.",
        StyledIcon("bug"),
        2500
    });

    //sentry_flush(2000);
    //sentry_shutdown();
}

void SentryErrorLogger::stop()
{
    if (!_isRunning)
        return;

    const auto dumpedEnvelopes = sentry_close();

    if (dumpedEnvelopes > 0)
        qWarning() << "Sentry shutdown timed out;" << dumpedEnvelopes << "envelope(s) were persisted for delivery during a later session";

    _isRunning = false;
    _handledExceptionSessionTimer.invalidate();
    _handledExceptionLastSent.clear();
    _handledExceptionsSent = 0;
}

void SentryErrorLogger::reportHandledException(const QString& title, const QString& exceptionType, const QString& reason, util::SeverityLevel severity, const util::StackTrace& stackTrace, const QString& diagnosticId, const QString& where)
{
    if (!_isRunning)
        return;

    const auto sanitizedReason = sanitizeForSentry(reason);
    const auto fingerprint = exceptionType + QLatin1Char('\n') + sanitizedReason;
    const auto now = _handledExceptionSessionTimer.elapsed();

    for (auto iterator = _handledExceptionLastSent.begin(); iterator != _handledExceptionLastSent.end();) {
        if (now - iterator.value() >= handledExceptionCooldownMilliseconds)
            iterator = _handledExceptionLastSent.erase(iterator);
        else
            ++iterator;
    }

    if (_handledExceptionsSent >= maximumHandledExceptionsPerSession || _handledExceptionLastSent.contains(fingerprint))
        return;

    _handledExceptionLastSent.insert(fingerprint, now);
    ++_handledExceptionsSent;

    auto event     = sentry_value_new_event();
    auto exception = sentry_value_new_exception(exceptionType.toUtf8().constData(), sanitizedReason.toUtf8().constData());
    auto sentryStackTrace = sentry_value_new_object();
    auto frames = sentry_value_new_list();
    auto sentryFingerprint = sentry_value_new_list();

    sentry_value_append(sentryFingerprint, sentry_value_new_string("{{ default }}"));
    sentry_value_append(sentryFingerprint, sentry_value_new_string("handled-exception"));
    sentry_value_set_by_key(event, "fingerprint", sentryFingerprint);

    // Sentry expects frames from the oldest call to the point where the
    // exception occurred; cpptrace provides them in the opposite order.
    for (auto frameIterator = stackTrace.crbegin(); frameIterator != stackTrace.crend(); ++frameIterator) {
        const auto& frame = *frameIterator;
        auto sentryFrame = sentry_value_new_object();

        if (!frame.function.isEmpty())
            sentry_value_set_by_key(sentryFrame, "function", sentry_value_new_string(frame.function.toUtf8().constData()));

        if (!frame.file.isEmpty())
            sentry_value_set_by_key(sentryFrame, "filename", sentry_value_new_string(QFileInfo(frame.file).fileName().toUtf8().constData()));

        if (frame.line >= 0)
            sentry_value_set_by_key(sentryFrame, "lineno", sentry_value_new_int32(frame.line));

        if (!frame.module.isEmpty())
            sentry_value_set_by_key(sentryFrame, "module", sentry_value_new_string(QFileInfo(frame.module).fileName().toUtf8().constData()));

        sentry_value_append(frames, sentryFrame);
    }

    sentry_value_set_by_key(sentryStackTrace, "frames", frames);
    sentry_value_set_by_key(exception, "stacktrace", sentryStackTrace);
    sentry_event_add_exception(event, exception);

    const auto level = [&severity]() {
        switch (severity) {
            case util::SeverityLevel::Info:    return "info";
            case util::SeverityLevel::Warning: return "warning";
            case util::SeverityLevel::Error:   return "error";
            case util::SeverityLevel::Fatal:   return "fatal";
        }

        return "error";
    }();

    sentry_value_set_by_key(event, "level", sentry_value_new_string(level));

    auto extra = sentry_value_new_object();

    if (!title.isEmpty())
        sentry_value_set_by_key(extra, "dialog_title", sentry_value_new_string(sanitizeForSentry(title).toUtf8().constData()));

    if (!diagnosticId.isEmpty())
        sentry_value_set_by_key(extra, "diagnostic_id", sentry_value_new_string(diagnosticId.toUtf8().constData()));

    if (!where.isEmpty())
        sentry_value_set_by_key(extra, "where", sentry_value_new_string(sanitizeForSentry(where).toUtf8().constData()));

    sentry_value_set_by_key(event, "extra", extra);
    sentry_capture_event(event);
}

bool SentryErrorLogger::submitUserFeedback(const QString& type, const QString& message, const QString& email)
{
    if (!_isRunning || message.trimmed().isEmpty())
        return false;

    const auto feedback = QString("[%1]\n%2").arg(type, message.trimmed()).toUtf8();
    const auto contact  = email.trimmed().toUtf8();

    sentry_capture_feedback(sentry_value_new_feedback(feedback.constData(), contact.isEmpty() ? nullptr : contact.constData(), nullptr, nullptr));
    return true;
}

QString SentryErrorLogger::getReleaseString()
{
    if (!getEnabledAction().isChecked())
        return {};

    const auto application = Application::current();

    if (application == nullptr)
        return {};

    return QString("ManiVaultStudio@%1").arg(QString::fromStdString(application->getVersion().getVersionString()));
}

bool SentryErrorLogger::isDsnValid() const
{
    QRegularExpressionValidator validator(getDsnAction().getValidator().regularExpression());

    auto errorLoggingDsn = getDsnAction().getString();
    int pos{};

    return validator.validate(errorLoggingDsn, pos) == QValidator::Acceptable;


}
