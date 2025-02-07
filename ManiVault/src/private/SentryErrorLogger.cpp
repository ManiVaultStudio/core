// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SentryErrorLogger.h"
#include "CrashReportDialog.h"

#include <ManiVaultVersion.h>

#include "sentry.h"

#include <QOperatingSystemVersion>

using namespace mv;
using namespace mv::gui;

SentryErrorLogger::SentryErrorLogger(QObject* parent /*= nullptr*/) :
    AbstractErrorLogger("Sentry", parent)
{
    //CrashReportDialog::initialize();
    //ErrorLoggingConsentDialog::setErrorLoggingInstance(this);
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
        start();
    }

    qDebug() << "Starting Sentry error logging...";

    if (!isDsnValid()) {
        qWarning() << "Cannot start Sentry: error logging is enabled, but the DSN is empty or invalid";
        return;
    }

	const auto dsn = getDsnAction().getString();

	sentry_options_t* options = sentry_options_new();
    
	sentry_options_set_dsn(options, dsn.toUtf8());
	sentry_options_set_handler_path(options, QString("%1/%2").arg(QDir::currentPath(), getCrashpadHandlerExecutableName()).toUtf8());
	sentry_options_set_database_path(options, ".sentry-native");

	const auto releaseString = getReleaseString().toUtf8();

#ifdef _DEBUG
    sentry_options_set_debug(options, 1);
    sentry_options_set_environment(options, "debug");
    sentry_options_set_release(options, releaseString + "-debug");
    sentry_set_tag("build_type", "debug");
#else
	sentry_options_set_debug(options, 0);
	sentry_options_set_environment(options, "release");
	sentry_options_set_release(options, releaseString + "-release");
	sentry_set_tag("build_type", "release");
#endif

    auto showCrashReportDialog = getShowCrashReportDialogAction().isChecked();

    sentry_options_set_before_send(options, [](sentry_value_t event, void* hint, void* closure) -> sentry_value_t {
        auto showCrashReportDialogAction = static_cast<bool*>(closure);

    	if (!*showCrashReportDialogAction)
            return event;

        CrashReportDialog crashReportDialog;

        if (crashReportDialog.exec() == QDialog::Accepted) {
            const auto crashUserInfo = crashReportDialog.getCrashUserInfo();

            if (crashUserInfo._sendReport) {
                sentry_value_t extra = sentry_value_new_object();

                sentry_value_set_by_key(extra, "feedback", sentry_value_new_string(crashUserInfo._feedback.toUtf8()));
                sentry_value_set_by_key(extra, "contactInfo", sentry_value_new_string(crashUserInfo._contactDetails.toUtf8()));

                sentry_value_set_by_key(event, "extra", extra);

                if (auto eventJson = sentry_value_to_json(event))
                    sentry_free(eventJson);
            }
        }

        return event;
    }, &showCrashReportDialog);

    if (sentry_init(options) == 0)
        qDebug() << "Sentry error logging is running, crash reports will send to: " + dsn;
    else
        qDebug() << "Sentry error logging is not running";

    addNotification("Started", {
        QString("%1 error logging").arg(getLoggerName()),
        "Error logging using <a href='https://sentry.io/'>Sentry</a> is active, crash reports will be logged to improve the application.",
        Application::getIconFont("FontAwesome").getIcon("bug"),
        2500
	});

    //sentry_flush(2000);
    //sentry_shutdown();
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
