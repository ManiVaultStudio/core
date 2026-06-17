// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "util/SeverityLevel.h"

#include <QObject>
#include <QVariantMap>

#include <source_location>

namespace mv {

class CORE_EXPORT ManiVaultException : public std::runtime_error {
public:
    ManiVaultException(util::SeverityLevel severity, QString message, QString what, QString where = {}, QVariantMap details = {}, std::source_location location = std::source_location::current());

    ManiVaultException withAddedDetails(const QVariantMap& additionalDetails) const;

    util::SeverityLevel     _severity;  /** The severity level of the exception */
    QString                 _message;   /** The message describing the exception */
    QString                 _what;      /** The specific cause or nature of the exception */
    QString                 _where;     /** The location or context where the exception occurred */
    QVariantMap             _details;   /** Additional details about the exception */
};

}
