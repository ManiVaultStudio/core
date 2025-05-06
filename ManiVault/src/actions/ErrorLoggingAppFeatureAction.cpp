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

    /*QFile file(":/HTML/AppFeatureErrorLogging");
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to load resource:" << file.errorString();
    }
    else {
        qDebug() << "Resource loaded successfully.";
    }*/
    getSummaryAction().setString("Send anonymous crash reports to improve the application");

    addAction(&_loggingDsnAction);
    addAction(&_loggingShowCrashReportDialogAction);
}

}
