// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SentryErrorLogger.h"
#include <ManiVaultVersion.h>

#include "sentry.h"

#include <QOperatingSystemVersion>
#include <QCoreApplication>
#include <QDir>
#include <QStandardPaths>

using namespace mv;
using namespace mv::gui;
using namespace mv::util;

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

    beginInitialization();
    {
    }
    endInitialization();
}

void SentryErrorLogger::start()
{
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
    sentry_options_set_handler_path(options, QDir(QCoreApplication::applicationDirPath()).filePath(getCrashpadHandlerExecutableName()).toUtf8());
    sentry_options_set_database_path(options, sentryDatabasePath.toUtf8());

    if (getShowCrashReportDialogAction().isChecked()) {
#ifdef Q_OS_WIN
        const auto crashReporterExecutable = "ManiVault Crash Reporter.exe";
#else
        const auto crashReporterExecutable = "ManiVault Crash Reporter";
#endif
        sentry_options_set_external_crash_reporter_path(options,
            QDir(QCoreApplication::applicationDirPath()).filePath(crashReporterExecutable).toUtf8());
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
        qDebug() << "Sentry error logging is not running";
        return;
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
    sentry_close();
}

QString SentryErrorLogger::getReleaseString()
{
    if (!getEnabledAction().isChecked())
        return {};

    const auto suffix = QString(MV_VERSION_SUFFIX.data());

    return QString("ManiVaultStudio@%1.%2.%3%4").arg(QString::number(MV_VERSION_MAJOR), QString::number(MV_VERSION_MINOR), QString::number(MV_VERSION_PATCH), suffix.isEmpty() ? "" : QString("-%1").arg(suffix));
}

bool SentryErrorLogger::isDsnValid() const
{
    QRegularExpressionValidator validator(getDsnAction().getValidator().regularExpression());

    auto errorLoggingDsn = getDsnAction().getString();
    int pos{};

    return validator.validate(errorLoggingDsn, pos) == QValidator::Acceptable;


}
