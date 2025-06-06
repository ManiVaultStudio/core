// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QMap>
#include <QString>

namespace mv::util
{

/** Script types */
enum class ScriptType {
    None = 0,               /** Not a valid script type */
    DataLoader,             /** Data loading scripts */
    DataWriter,             /** Data writer scripts */
    DataAnalysis,           /** Data analysis scripts */
    DataTransformation,     /** Data transformation scripts */
    DataView                /** Data view scripts */
};

/** Maps script type to script type name */
static QMap<ScriptType, QString> scriptTypeMap({
    { ScriptType::DataLoader, "DataLoader" },
    { ScriptType::DataWriter, "DataWriter" },
    { ScriptType::DataAnalysis, "DataAnalysis" },
    { ScriptType::DataTransformation, "DataTransformation" },
    { ScriptType::DataView, "DataView" }
});

/**
 * Get the script type name from \p scriptType
 * @param scriptType Script type
 * @return Script type name
 */
QString getScriptTypeName(const ScriptType& scriptType);

/**
 * Get script type from \p scriptTypeName
 * @param scriptTypeName Script type name
 * @return Script type
 */
ScriptType getScriptType(const QString& scriptTypeName);

}
