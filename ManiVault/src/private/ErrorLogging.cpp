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

ErrorLogging::ErrorLogging(QObject* parent /*= nullptr*/) :
    QObject(parent),
    _initialized(false),
    _userHasOptedAction(this, "User has opted", false),
    _enabledAction(this, "Toggle error reporting", false),
    _dsnAction(this, "Sentry DSN"),
	_showCrashReportDialogAction(this, "Show crash report dialog", true)
{
    CrashReportDialog::initialize();
    ErrorLoggingConsentDialog::setErrorLoggingInstance(this);

    _userHasOptedAction.setSettingsPrefix("ErrorLogging/UserHasOpted");
    _enabledAction.setSettingsPrefix("ErrorLogging/Enabled");
    _dsnAction.setSettingsPrefix("ErrorLogging/DSN");
    _showCrashReportDialogAction.setSettingsPrefix("ErrorLogging/DSN");

    _dsnAction.setToolTip("The Sentry error logging data source name");
    _dsnAction.getValidator().setRegularExpression(QRegularExpression(R"(^https?://[a-f0-9]{32}@[a-z0-9\.-]+(:\d+)?/[\d]+$)"));

    _notificationTimer.setSingleShot(true);
    _notificationTimer.setInterval(1000);

    connect(&_notificationTimer, &QTimer::timeout, this, [this]() -> void {
        mv::help().addNotification("Error logging", "The changes will be applied after restarting the application.", Application::getIconFont("FontAwesome").getIcon("bug"));
    });

    connect(&_enabledAction, &ToggleAction::toggled, this, [this](bool enabled) -> void {
        ////const auto settingsDirty = !Application::current()->hasSetting(enabledSettingsKey) || enabled != getErrorLoggingEnabled();

        //Application::current()->setSetting(enabledSettingsKey, enabled);

        //if (mv::core()->isInitialized())
        //    mv::settings().getApplicationSettings().getErrorLoggingEnabledAction().setChecked(enabled);

        ////if (settingsDirty)
        //    setSettingsDirty();
    });

    connect(&_dsnAction, &ToggleAction::toggled, this, [this]() -> void {
        //const auto settingsDirty = !Application::current()->hasSetting(enabledSettingsKey) || errorLoggingDsn != getErrorLoggingDsn();

        //Application::current()->setSetting(dsnSettingsKey, errorLoggingDsn);

        //mv::settings().getApplicationSettings().getErrorLoggingDsnAction().setString(errorLoggingDsn);

        //if (settingsDirty)
        //    setSettingsDirty();
    });

    if (!_userHasOptedAction.isChecked()) {

        ErrorLoggingConsentDialog errorLoggingConsentDialog;

        errorLoggingConsentDialog.exec();

        if (_enabledAction.isChecked())
            start();
    }
}

QString ErrorLogging::getCrashpadHandlerExecutableName() const
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

void ErrorLogging::initialize()
{
    if (ErrorLogging::_initialized)
        return;

    auto& applicationSettingsAction = mv::settings().getApplicationSettings();

    applicationSettingsAction.getErrorLoggingEnabledAction().setChecked(getEnabledAction().isChecked());
    //applicationSettingsAction.getErrorLoggingDsnAction().setString(getErrorLoggingDsn());

    //
    //connect(&applicationSettingsAction.getErrorLoggingDsnAction(), &StringAction::stringChanged, this, &ErrorLogging::setErrorLoggingDsn);

    /*connect(&applicationSettingsAction.getErrorLoggingConsentAction(), &TriggerAction::triggered, []() -> void {
        ErrorLoggingConsentDialog errorLoggingConsentDialog;

        errorLoggingConsentDialog.exec();
	});*/

    connect(&getEnabledAction(), &ToggleAction::toggled, this, [this](bool enabled) -> void {
		mv::settings().getApplicationSettings().getErrorLoggingEnabledAction().setChecked(enabled);

        setSettingsDirty();
	});

    connect(&applicationSettingsAction.getErrorLoggingEnabledAction(), &ToggleAction::toggled, &getEnabledAction(), &ToggleAction::setChecked);

    if (getUserHasOptedAction().isChecked() && getEnabledAction().isChecked())
		start();

    _initialized = true;
}

void ErrorLogging::start()
{
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

        CrashReportDialog dialog;

        if (dialog.exec() == QDialog::Accepted) {
            const auto crashUserInfo = dialog.getCrashUserInfo();

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

    //sentry_flush(2000);
    //sentry_shutdown();
}

QString ErrorLogging::getReleaseString()
{
    if (!getEnabledAction().isChecked())
        return {};

    const auto suffix = QString(MV_VERSION_SUFFIX.data());

    return QString("ManiVaultStudio@%1.%2.%3%4").arg(QString::number(MV_VERSION_MAJOR), QString::number(MV_VERSION_MINOR), QString::number(MV_VERSION_PATCH), suffix.isEmpty() ? "" : QString("-%1").arg(suffix));
}

void ErrorLogging::setSettingsDirty()
{
	_notificationTimer.start();
}

bool ErrorLogging::isDsnValid() const
{
    QRegularExpressionValidator validator(const_cast<mv::gui::StringAction&>(getDsnAction()).getValidator().regularExpression());

    auto errorLoggingDsn = getDsnAction().getString();
    int pos{};

    return validator.validate(errorLoggingDsn, pos) == QValidator::Acceptable;


}
