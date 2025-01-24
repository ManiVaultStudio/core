// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ErrorLogging.h"

#include "CrashReportDialog.h"
#include "ErrorLoggingConsentDialog.h"

#include <Application.h>
#include <ManiVaultVersion.h>

#include "sentry.h"

using namespace mv;
using namespace mv::gui;

const QString ErrorLogging::userHasOptedSettingsKey = QString("ErrorLogging/UserHasOpted");
const QString ErrorLogging::enabledSettingsKey = QString("ErrorLogging/Enabled");

ErrorLogging::ErrorLogging(QObject* parent /*= nullptr*/) :
    QObject(parent)
{
    initialize();
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
    Application::current()->setSetting(enabledSettingsKey, errorLoggingEnabled);
}

void ErrorLogging::initialize()
{
    //connect(&mv::settings().getApplicationSettings().getAllowErrorReportingAction(), &ToggleAction::toggled, &ErrorLogging::setEnabled);

    CrashReportDialog::initialize();

    if (!getUserHasOpted()) {

        ErrorLoggingConsentDialog errorLoggingConsentDialog;

        errorLoggingConsentDialog.exec();
    }
}

void ErrorLogging::setEnabled(bool enabled)
{
    if (enabled)
		qDebug() << "Enabling error logging with Sentry";
    else
        qDebug() << "Disabling error logging with Sentry";

    Application::current()->setSetting(enabledSettingsKey, enabled);

	if (enabled) {
		sentry_options_t* options = sentry_options_new();

		sentry_options_set_dsn(options, "https://a7cf606b5e26f698d8980a15d39262d6@o4508081578442752.ingest.de.sentry.io/4508681697099856");
		sentry_options_set_handler_path(options, QString("%1/crashpad_handler.exe").arg(QDir::currentPath()).toLatin1());
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
				if (!mv::settings().getApplicationSettings().getShowErrorReportDialogAction().isChecked())
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
		sentry_shutdown();
	}
}

QString ErrorLogging::getReleaseString()
{
    if (!getErrorLoggingEnabled())
        return {};

    return QString("ManiVaultStudio@%1.%2.%3").arg(QString::number(MV_VERSION_MAJOR), QString::number(MV_VERSION_MINOR), QString::number(MV_VERSION_PATCH), QString(MV_VERSION_SUFFIX.data()));
}
