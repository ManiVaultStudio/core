// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ErrorManager.h"
#include "ErrorLoggingConsentDialog.h"

using namespace mv::gui;
using namespace mv::util;

#ifdef _DEBUG
    //#define ERROR_MANAGER_VERBOSE
#endif

#ifdef ERROR_LOGGING
	#include "private/SentryErrorLogger.h"
#endif

namespace mv
{

ErrorManager::ErrorManager(QObject* parent) :
    AbstractErrorManager(parent),
    _loggingAskConsentDialogAction(this, "Consent..."),
    _loggingUserHasOptedAction(this, "User has opted", false),
    _loggingEnabledAction(this, "Toggle error reporting", false),
    _loggingDsnAction(this, "Sentry DSN"),
    _loggingShowCrashReportDialogAction(this, "Show crash report dialog")
{
    _loggingAskConsentDialogAction.setToolTip("Show the error logging consent dialog");
    _loggingAskConsentDialogAction.setDefaultWidgetFlags(TriggerAction::IconText);

    //_loggingUserHasOptedAction.setSettingsPrefix(QString("%1Logging/UserHasOpted").arg(getSettingsPrefix()));
    _loggingUserHasOptedAction.setToolTip("Whether the user has opted in or out");

    //_loggingEnabledAction.setSettingsPrefix(QString("%1Logging/Enabled").arg(getSettingsPrefix()));
    _loggingEnabledAction.setToolTip("Toggle Sentry error logging");

#ifdef _DEBUG
    _loggingEnabledAction.setEnabled(false);
#endif
	
    //_loggingDsnAction.setSettingsPrefix(QString("%1Logging/DSN").arg(getSettingsPrefix()));
    _loggingDsnAction.setToolTip("The Sentry error logging data source name");
    _loggingDsnAction.getValidator().setRegularExpression(QRegularExpression(R"(^https?://[a-f0-9]{32}@[a-z0-9\.-]+(:\d+)?/[\d]+$)"));

    //_loggingShowCrashReportDialogAction.setSettingsPrefix(QString("%1Logging/ShowCrashReportDialog").arg(getSettingsPrefix()));
    _loggingShowCrashReportDialogAction.setToolTip("Show the crash report dialog prior to sending an error report");

    const auto allowErrorReportingChanged = [this]() -> void {
        _loggingShowCrashReportDialogAction.setEnabled(_loggingEnabledAction.isChecked());
        _loggingDsnAction.setEnabled(_loggingEnabledAction.isChecked());
	};

    allowErrorReportingChanged();

    connect(&_loggingEnabledAction, &ToggleAction::toggled, this, allowErrorReportingChanged);
}

ErrorManager::~ErrorManager()
{
    reset();
}

void ErrorManager::initialize()
{
#ifdef ERROR_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    AbstractErrorManager::initialize();

    if (isInitialized())
        return;

    beginInitialization();
    {
        auto& errorLoggingSettingsAction = mv::settings().getErrorLoggingSettingsAction();

        errorLoggingSettingsAction.addAction(&getLoggingAskConsentDialogAction());
        errorLoggingSettingsAction.addAction(&getLoggingEnabledAction());
        errorLoggingSettingsAction.addAction(&getLoggingDsnAction());
        errorLoggingSettingsAction.addAction(&getLoggingShowCrashReportDialogAction());

#ifdef ERROR_LOGGING
        setErrorLogger(new SentryErrorLogger(this));

    	getErrorLogger()->initialize();

    	connect(&getLoggingAskConsentDialogAction(), &TriggerAction::triggered, this, &ErrorManager::showErrorLoggingConsentDialog);

        if (!getLoggingUserHasOptedAction().isChecked())
            showErrorLoggingConsentDialog();

        getErrorLogger()->requestStart();
#endif
    }
    endInitialization();
}

void ErrorManager::reset()
{
#ifdef ERROR_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    beginReset();
    {
    }
    endReset();
}

void ErrorManager::showErrorLoggingConsentDialog()
{
#ifdef ERROR_LOGGING
    ErrorLoggingConsentDialog errorLoggingConsentDialog;
    errorLoggingConsentDialog.exec();
#endif
}

}
