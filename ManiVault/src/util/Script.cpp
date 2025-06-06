// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Script.h"

namespace mv::util
{

QString getScriptTypeName(const ScriptType& scriptType)
{
	return scriptTypeMap.value(scriptType, "None");
}

ScriptType getScriptType(const QString& typeName)
{
	const auto scriptTypes = scriptTypeMap.keys(typeName);

	if (scriptTypes.isEmpty())
		return ScriptType::None; // Default to None if typeName is not found

	return scriptTypes.first();
}

}
