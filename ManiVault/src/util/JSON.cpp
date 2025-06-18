// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "JSON.h"

#include <nlohmann/json-schema.hpp>

#include <regex>

#include <QString>

using nlohmann::json;
using namespace nlohmann::json_schema;
using nlohmann::json_schema::json_validator;

namespace mv::util {

/**
 * JSON schema error handler class
 *
 * Custom error handling for JSON schema
 *
 * @author Thomas Kroes
 */
class JsonSchemaErrorHandler : public nlohmann::json_schema::basic_error_handler
{
private:

    struct SchemaValidationError
    {
        nlohmann::json::json_pointer    _pointer;
        nlohmann::json                  _instance;
        std::string                     _message;
    };

public:

    /**
     * Construct with json schema and json file location
     * @param jsonSchemaLocation Location of the JSON validation schema file
     * @param publicJsonSchemaLocation Location of the public JSON validation schema file
     * @param jsonLocation Location of the validated JSON file
     */
    JsonSchemaErrorHandler(const std::string& jsonSchemaLocation, const std::string& publicJsonSchemaLocation, const std::string& jsonLocation);

    /** Show a dialog with errors when going out of scope */
    virtual ~JsonSchemaErrorHandler();

	/**
     * Invoked when an error occurs during JSON schema validation
     * @param pointer JSON pointer to the error location
     * @param instance JSON instance that caused the error
     * @param message Error message describing the issue
	 */
	void error(const nlohmann::json::json_pointer& pointer, const nlohmann::json& instance, const std::string& message) override
	{
	    basic_error_handler::error(pointer, instance, message);

        _errors.push_back({ pointer, instance, message });
	}

    /**
     * Truncate JSON values to not pollute JSON logging
     * @param json JSON content
     * @param maxStringLen Maximum string length
     * @return Truncated JSON
     */
    static nlohmann::json truncateJsonForLogging(const nlohmann::json& json, size_t maxStringLen = 60);

private:

    /** Print encountered errors s*/
    void printErrors() const;

private:
    std::string                         _jsonSchemaLocation;        /** Location of the JSON validation schema file */
    std::string                         _publicJsonSchemaLocation;  /** Location of the public JSON validation schema file */
    std::string                         _jsonLocation;              /** Location of the validated JSON file */
    std::vector<SchemaValidationError>  _errors;                    /** List of errors encountered during validation */
};


void printIndentedJson(const nlohmann::json& json, std::int32_t indentLevel = 3) {
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

JsonSchemaErrorHandler::JsonSchemaErrorHandler(const std::string& jsonSchemaLocation, const std::string& publicJsonSchemaLocation, const std::string& jsonLocation) :
    _jsonSchemaLocation(jsonSchemaLocation),
    _publicJsonSchemaLocation(publicJsonSchemaLocation),
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
        qDebug() << "  " << _jsonLocation << "is valid!";
    } else {
        qCritical() << "  JSON file is not valid:";

        for (const auto& error : _errors) {
            qCritical() << "    Schema error:";
            qCritical() << "      Schema:" << _publicJsonSchemaLocation;
            qCritical() << "      Location: " << error._pointer.to_string();
            qCritical().noquote() << "      Instance:";
        	printIndentedJson(truncateJsonForLogging(error._instance));
            qCritical() << "      Message: " << error._message;
        }
    }
}

json loadJsonFromResource(const std::string& resourcePath) {
    QFile file(QString::fromStdString(resourcePath));

    if (!file.open(QIODevice::ReadOnly)) {
        throw std::runtime_error("Failed to open resource: " + resourcePath);
    }

    QByteArray data = file.readAll();

    return nlohmann::json::parse(data.constData());
}

void validateJsonWithResourceSchema(const nlohmann::json& json, const std::string& jsonLocation, const std::string& jsonSchemaResourcePath, const std::string& publicJsonSchemaLocation /*= ""*/)
{
    const auto jsonSchema = loadJsonFromResource(jsonSchemaResourcePath);

    json_validator jsonValidator;

    try {
        jsonValidator.set_root_schema(jsonSchema);
    }
    catch (const std::exception& e) {
        qCritical() << "Unable to assign JSON validator schema: " << e.what() << "\n";
        return;
    }

    jsonValidator.set_root_schema(jsonSchema);

    JsonSchemaErrorHandler jsonSchemaErrorHandler(jsonSchemaResourcePath, publicJsonSchemaLocation, jsonLocation);

    jsonValidator.validate(json, jsonSchemaErrorHandler);
}

}
