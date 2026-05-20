// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QObject>

using namespace mv::util;

namespace mv {

class CORE_EXPORT ManiVaultException : public std::runtime_error {
public:
    ManiVaultException(SeverityLevel severity, QString message, QString what, QString where = {}, QVariantMap details = {});

    ManiVaultException withAddedDetails(const QVariantMap& additionalDetails) const;

    SeverityLevel   _severity;  /** The severity level of the exception */
    QString         _message;   /** The message describing the exception */
    QString         _what;      /** The specific cause or nature of the exception */
    QString         _where;     /** The location or context where the exception occurred */
    QVariantMap     _details;   /** Additional details about the exception */
};

}
