// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QObject>

namespace mv::util {

class CORE_EXPORT ManiVaultException : public std::runtime_error {
public:
    ManiVaultException(
        SeverityLevel severity,
        QString message,
        QString code,
        QString scope = {},
        QVariantMap details = {}
    );

    SeverityLevel _severity;
    QString _message;
    QString _code;
    QString _scope;
    QVariantMap _details;
};

}
