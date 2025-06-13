// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "JSON.h"

#include <regex>

using nlohmann::json;
using namespace nlohmann::json_schema;
using nlohmann::json_schema::json_validator;

namespace mv::util {

void printIndentedJson(const nlohmann::json& json, std::int32_t indentLevel = 2) {
    QString prefix(indentLevel * 2, ' ');

    const auto dumped   = json.dump(2);
	const auto lines    = QString::fromStdString(dumped).split('\n');

    for (const QString& line : lines)
        qCritical().noquote() << prefix + line;
}

json JsonSchemaErrorHandler::truncateJsonForLogging(const json& json, size_t maxStringLen /*= 60*/) {
    if (json.is_object()) {
        auto result = json::object();

        for (auto it = json.begin(); it != json.end(); ++it)
            result[it.key()] = truncateJsonForLogging(it.value(), maxStringLen);
        
        return result;
    }

	if (json.is_array()) {
        auto result = json::array();

        for (const auto& item : json)
            result.push_back(truncateJsonForLogging(item, maxStringLen));
        
        return result;
    }

	if (json.is_string()) {
        const auto& str = json.get_ref<const std::string&>();

        if (str.length() > maxStringLen) {
            return str.substr(0, maxStringLen) + "... (truncated)";
        }
        
        return str;
    }

	return json;
}

JsonSchemaErrorHandler::JsonSchemaErrorHandler(const std::string& jsonSchemaLocation, const std::string& jsonLocation) :
    _jsonSchemaLocation(jsonSchemaLocation),
    _jsonLocation(jsonLocation)
{
    qDebug() << "Validating" << _jsonLocation << "against" << _jsonSchemaLocation;
}

JsonSchemaErrorHandler::~JsonSchemaErrorHandler()
{
    printErrors();
}

void JsonSchemaErrorHandler::printErrors() const
{
    if (_errors.empty()) {
        qDebug() << _jsonLocation << "validated successfully with" << _jsonSchemaLocation;
    } else {
        qCritical() << "Encountered schema validation errors for " << _jsonLocation;

        for (const auto& error : _errors) {
            qCritical() << "  Schema Validation Error:";
            qCritical() << "    Location: " << error._pointer.to_string();
            qCritical().noquote() << "    Instance:";
        	printIndentedJson(truncateJsonForLogging(error._instance));
            qCritical() << "    Message  : " << error._message;
        }
    }
}

json loadJsonFromResource(const QString& resourcePath) {
    QFile file(resourcePath);

    if (!file.open(QIODevice::ReadOnly)) {
        throw std::runtime_error("Failed to open resource: " + resourcePath.toStdString());
    }

    QByteArray data = file.readAll();

    return nlohmann::json::parse(data.constData());
}

void validateJsonWithResourceSchema(const nlohmann::json& json, const QString& jsonLocation, const QString& resourcePath)
{
    const auto jsonSchema = loadJsonFromResource(resourcePath);

    json_validator jsonValidator;

    try {
        jsonValidator.set_root_schema(jsonSchema);
    }
    catch (const std::exception& e) {
        qCritical() << "Unable to assign JSON validator schema: " << e.what() << "\n";
        return;
    }

    jsonValidator.set_root_schema(jsonSchema);

    JsonSchemaErrorHandler jsonSchemaErrorHandler(jsonLocation.toStdString(), resourcePath.toStdString());

    jsonValidator.validate(json, jsonSchemaErrorHandler);
}

}
