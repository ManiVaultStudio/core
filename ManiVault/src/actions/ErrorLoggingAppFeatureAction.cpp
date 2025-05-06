// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ErrorLoggingAppFeatureAction.h"

namespace mv::gui {

ErrorLoggingAppFeatureAction::ErrorLoggingAppFeatureAction(QObject* parent, const QString& title /*= "Error Logging"*/) :
    AppFeatureAction(parent, title)
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
}

}
