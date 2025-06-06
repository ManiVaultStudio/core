// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Script.h"

namespace mv::util
{

QMap<Script::Type, QString> Script::typeMap = {
    { Type::None, "None" },
    { Type::DataLoader, "Data Loader" },
    { Type::DataWriter, "Data Writer" },
    { Type::DataAnalysis, "Data Analysis" },
    { Type::DataTransformation, "Data Transformation" },
    { Type::DataView, "Data View" }
};

QString Script::getTypeName(const Type& type)
{
	return typeMap.value(type, "None");
}

Script::Type Script::getTypeEnum(const QString& typeName)
{
	const auto scriptTypes = typeMap.keys(typeName);

	if (scriptTypes.isEmpty())
		return Type::None; // Default to None if typeName is not found

	return scriptTypes.first();
}

QString Script::getLanguageName(const Language& language)
{
    return languageMap.value(language, "None");
}

Script::Language Script::getLanguageEnum(const QString& languageName)
{
    const auto languagesEnums = languageMap.keys(languageName);

    if (languagesEnums.isEmpty())
        return Language::None; // Default to None if languageName is not found

    return languagesEnums.first();
}

Script::Script(const Type& type, const Language& language, const QUrl& filePath, QObject* parent /*= nullptr*/) :
    QObject(parent),
    _type(type),
    _language(language),
    _filePath(filePath)
{
}

}
