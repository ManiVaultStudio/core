// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Script.h"

#include "util/StyledIcon.h"

#ifdef _DEBUG
    #define SCRIPT_VERBOSE
#endif

namespace mv::util
{

QMap<Script::Type, QString> Script::typeMap = {
    { Type::None, "None" },
    { Type::Loader, "Loader" },
    { Type::Writer, "Writer" },
    { Type::Analysis, "Analysis" },
    { Type::Transformation, "Transformation" },
    { Type::View, "View" }
};

QMap<Script::Language, QString> Script::languageMap = {
    { Language::None, "None" },
    { Language::Python, "Python" },
    { Language::R, "R" }
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

Script::Script(const QString& title, const Type& type, const Language& language, const Version& languageVersion, const QString& location, QObject* parent /*= nullptr*/) :
    QObject(parent),
    _title(title),
    _type(type),
    _language(language),
    _languageVersion(languageVersion),
    _location(location),
    _datasets(),
    _dataTypes()
{
}

Script::Script(const QString& title, const Type& type, const Language& language, const Version& languageVersion, const QString& location, const Datasets& datasets, QObject* parent /*= nullptr*/) :
    QObject(parent),
    _title(title),
    _type(type),
    _language(language),
    _languageVersion(languageVersion),
    _location(location),
    _datasets(datasets),
    _dataTypes()
{
}

Script::Script(const QString& title, const Type& type, const Language& language, const Version& languageVersion, const QString& location, const DataTypes& dataTypes, QObject* parent /*= nullptr*/) :
    QObject(parent),
    _title(title),
    _type(type),
    _language(language),
    _languageVersion(languageVersion),
    _location(location),
    _datasets(),
    _dataTypes(dataTypes)
{
}

void Script::run()
{
#ifdef SCRIPT_VERBOSE
    qDebug().noquote() << QString("Running %1 script %2 (%3): %4").arg(util::Script::getTypeName(getType()), getTitle(), util::Script::getLanguageName(getLanguage()), getLocation());
#endif
}

QIcon Script::getLanguageIcon() const
{
    switch (_language) {
        case Language::Python:
            return StyledIcon("python");

        case Language::R:
            return StyledIcon("r");

        case Language::None:
            break;
    }

    return StyledIcon("play");
}

}
