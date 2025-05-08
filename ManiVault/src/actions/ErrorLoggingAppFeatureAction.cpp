// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ErrorLoggingAppFeatureAction.h"

namespace mv::gui {

ErrorLoggingAppFeatureAction::ErrorLoggingAppFeatureAction(QObject* parent, const QString& title /*= "Error Logging"*/) :
    AppFeatureAction(parent, title),
    _loggingDsnAction(this, "Sentry DSN", "https://211289c773dcc267b1bb536b6c3a23f7@lkebsentry.nl/2"),
    _loggingShowCrashReportDialogAction(this, "Show crash report dialog", true)
{
    loadDescriptionFromResource(":/HTML/AppFeatureErrorLogging");

    getSummaryAction().setString("Send anonymous crash reports to improve the application");

    _loggingDsnAction.setSettingsPrefix(QString("%1/DSN").arg(WidgetAction::getSettingsPrefix()));
    _loggingDsnAction.setToolTip("The Sentry error logging data source name");
    _loggingDsnAction.getValidator().setRegularExpression(QRegularExpression(R"(^https?://[a-f0-9]{32}@[a-z0-9\.-]+(:\d+)?/[\d]+$)"));

    _loggingShowCrashReportDialogAction.setSettingsPrefix(QString("%1/ShowCrashReportDialog").arg(WidgetAction::getSettingsPrefix()));
    _loggingShowCrashReportDialogAction.setToolTip("Show the crash report dialog prior to sending an error report");

    addAction(&_loggingDsnAction);
    addAction(&_loggingShowCrashReportDialogAction);

    const auto allowErrorReportingChanged = [this]() -> void {
        _loggingShowCrashReportDialogAction.setEnabled(getEnabledAction().isChecked());
        _loggingDsnAction.setEnabled(getEnabledAction().isChecked());
        };

    allowErrorReportingChanged();

    connect(&getEnabledAction(), &ToggleAction::toggled, this, allowErrorReportingChanged);
}

}
