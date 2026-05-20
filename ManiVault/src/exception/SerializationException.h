// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "ManiVaultException.h"

#include <QObject>

namespace mv::util {

class CORE_EXPORT SerializationException : public ManiVaultException {
public:
    SerializationException(SeverityLevel severity, const QString& message, const QString& code, const QString& scope = {}, QVariantMap details = {});

public: // Convenience helpers

    static SerializationException missingKey(const QString& key, const QString& scope, const QVariantMap& map);
};

}
