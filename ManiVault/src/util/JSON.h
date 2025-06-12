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
	/**
     * Invoked when an error occurs during JSON schema validation
     * @param pointer JSON pointer to the error location
     * @param instance JSON instance that caused the error
     * @param message Error message describing the issue
	 */
	void error(const nlohmann::json::json_pointer& pointer, const nlohmann::json& instance, const std::string& message) override
	{
	    nlohmann::json_schema::basic_error_handler::error(pointer, instance, message);
	    std::cerr << "ERROR: '" << pointer << "' - '" << instance << "': " << message << "\n";
	}
};

/**
 * Load JSON from a resource file
 * @param resourcePath Path to the JSON resource file
 * @return Parsed JSON object
 */
CORE_EXPORT nlohmann::json loadJsonFromResource(const QString& resourcePath);

/**
 * Validate JSON content against a schema defined in a resource file
 * @param json JSON content as a string
 * @param resourcePath Path to the JSON resource file
 */
CORE_EXPORT void validateJsonWithResourceSchema(const QString& json, const QString& resourcePath);

}