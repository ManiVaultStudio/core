// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SerializationException.h"

#include <QDebug>

namespace mv::util {

SerializationException::SerializationException(SeverityLevel severity, const QString& message, const QString& code, const QString& scope, QVariantMap details) :
	ManiVaultException(severity, message, code.startsWith("serialization.") ? code : QString("serialization.%1").arg(code), scope, std::move(details))
{
}

SerializationException SerializationException::missingKey(const QString& key, const QString& scope, const QVariantMap& map)
{
    QVariantMap details;

    details["missingKey"]       = key;
    details["availableKeys"]    = map.keys();
    details["variantMap"]       = map;

    return SerializationException(
        SeverityLevel::Error,
        QString("Required key '%1' was not found.").arg(key),
        "missing_key",
        scope,
        details
    );
}

}
