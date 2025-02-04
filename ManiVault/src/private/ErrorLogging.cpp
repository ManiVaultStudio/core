// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ErrorLogging.h"

#include "CrashReportDialog.h"
#include "ErrorLoggingConsentDialog.h"

#include <Application.h>
#include <ManiVaultVersion.h>

#include "sentry.h"

#include <QOperatingSystemVersion>

using namespace mv;
using namespace mv::gui;

bool ErrorLogging::initialized = false;
const QString ErrorLogging::userHasOptedSettingsKey = QString("ErrorLogging/UserHasOpted");
const QString ErrorLogging::enabledSettingsKey = QString("ErrorLogging/Enabled");

ErrorLogging::ErrorLogging(QObject* parent /*= nullptr*/) :
    QObject(parent)
{
    CrashReportDialog::initialize();

    if (!getUserHasOpted()) {

        ErrorLoggingConsentDialog errorLoggingConsentDialog;

        errorLoggingConsentDialog.exec();
    }
}

bool ErrorLogging::getUserHasOpted()
{
    return Application::current()->getSetting(userHasOptedSettingsKey, false).toBool();
}

void ErrorLogging::setUserHasOpted(bool userHasOpted)
{
    Application::current()->setSetting(userHasOptedSettingsKey, userHasOpted);
}

bool ErrorLogging::getErrorLoggingEnabled()
{
    return Application::current()->getSetting(enabledSettingsKey, false).toBool();
}

void ErrorLogging::setErrorLoggingEnabled(bool errorLoggingEnabled)
{
    setEnabled(errorLoggingEnabled);
}

QString ErrorLogging::getCrashpadHandlerExecutableName()
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
            return "";
    }
}

void ErrorLogging::initialize()
{
    if (ErrorLogging::initialized)
        return;

    connect(&mv::settings().getApplicationSettings().getAllowErrorLoggingAction(), &ToggleAction::toggled, [](bool toggled) -> void {
        ErrorLogging::setErrorLoggingEnabled(toggled);
	});

    connect(&mv::settings().getApplicationSettings().getErrorLoggingDsnAction(), &StringAction::stringChanged, [](const QString& errorLoggingDsn) -> void {
        if (getErrorLoggingEnabled()) {
	        ErrorLogging::setErrorLoggingEnabled(false);
			ErrorLogging::setErrorLoggingEnabled(true);
        }
	});

    connect(&mv::settings().getApplicationSettings().getErrorLoggingConsentAction(), &TriggerAction::triggered, []() -> void {
        ErrorLoggingConsentDialog errorLoggingConsentDialog;

        errorLoggingConsentDialog.exec();
	});

    if (getUserHasOpted())
		setEnabled(getErrorLoggingEnabled(), true);

    ErrorLogging::initialized = true;
}

void ErrorLogging::setEnabled(bool enabled, bool force /*= false*/)
{
    if (enabled == getErrorLoggingEnabled() && !force)
        return;

    Application::current()->setSetting(enabledSettingsKey, enabled);

    mv::settings().getApplicationSettings().getAllowErrorLoggingAction().setChecked(getErrorLoggingEnabled());

	const auto dsn = mv::settings().getApplicationSettings().getErrorLoggingDsnAction().getString();

    if (!dsn.isEmpty()) {
		if (enabled) {
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

			sentry_options_set_before_send(options, [](sentry_value_t event, void* hint, void* userdata) -> sentry_value_t {
					if (!mv::settings().getApplicationSettings().getShowCrashReportDialogAction().isChecked())
						return event;

					CrashReportDialog dialog;

					if (dialog.exec() == QDialog::Accepted) {
						const auto crashUserInfo = dialog.getCrashUserInfo();

						if (crashUserInfo._sendReport) {
							sentry_value_t extra = sentry_value_new_object();

							sentry_value_set_by_key(extra, "feedback", sentry_value_new_string(crashUserInfo._feedback.toUtf8()));
							sentry_value_set_by_key(extra, "contactInfo", sentry_value_new_string(crashUserInfo._contactDetails.toUtf8()));

							sentry_value_set_by_key(event, "extra", extra);

	                        if (mv::projects().hasProject() && mv::projects().getCurrentProject()->getReadOnlyAction().isChecked())
								sentry_set_tag("application", mv::projects().getCurrentProject()->getTitleAction().getString().toUtf8());

							if (auto eventJson = sentry_value_to_json(event))
								sentry_free(eventJson);
						}
					}

					return event;
				},
				nullptr
			);

			sentry_init(options);
		}
		else {
	        sentry_flush(2000);
			sentry_shutdown();
		}

        if (enabled)
            qDebug() << "Sentry error logging is running";
        else
            qDebug() << "Sentry error logging is not running";
    }
    else {
        qWarning() << "Cannot start Sentry: error logging is enabled, but the DSN is empty or invalid";
    }
}

QString ErrorLogging::getReleaseString()
{
    if (!getErrorLoggingEnabled())
        return {};

    const auto suffix = QString(MV_VERSION_SUFFIX.data());

    return QString("ManiVaultStudio@%1.%2.%3%4").arg(QString::number(MV_VERSION_MAJOR), QString::number(MV_VERSION_MINOR), QString::number(MV_VERSION_PATCH), suffix.isEmpty() ? "" : QString("-%1").arg(suffix));
}
