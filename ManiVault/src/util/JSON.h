// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QString>

#include <nlohmann/json.hpp>
#include <nlohmann/json-schema.hpp>

namespace mv::util {

/**
 * JSON schema error handler class
 *
 * Custom error handling for JSON schema
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT JsonSchemaErrorHandler : public nlohmann::json_schema::basic_error_handler
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
     * @param jsonLocation Location of the validated JSON file
     */
    JsonSchemaErrorHandler(const std::string& jsonSchemaLocation, const std::string& jsonLocation);

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
    std::string                         _jsonSchemaLocation;    /** Location of the JSON validation schema file */
    std::string                         _jsonLocation;          /** Location of the validated JSON file */
    std::vector<SchemaValidationError>  _errors;                /** List of errors encountered during validation */
};

/**
 * Load JSON from a resource file
 * @param resourcePath Path to the JSON resource file
 * @return Parsed JSON object
 */
CORE_EXPORT nlohmann::json loadJsonFromResource(const QString& resourcePath);

/**
 * Validate JSON content against a schema defined in a resource file
 * @param json JSON content
 * @param jsonLocation Location of the JSON file
 * @param resourcePath Path to the JSON resource file
 */
CORE_EXPORT void validateJsonWithResourceSchema(const nlohmann::json& json, const QString& jsonLocation, const QString& resourcePath);

}