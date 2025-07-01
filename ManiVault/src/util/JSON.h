// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

namespace mv::util {

/**
 * Check if a string is valid JSON
 * @param input String to check
 * @return True if the string is valid JSON, false otherwise
 */
CORE_EXPORT bool isValidJson(const std::string& input);

/**
 * Load JSON from a resource file
 * @param resourcePath Path to the JSON resource file
 * @return Parsed JSON content as a QString
 */
CORE_EXPORT std::string loadJsonFromResource(const std::string& resourcePath);

/**
 * Validate JSON string content against a schema defined in a resource file as a JSON string
 * @param jsonString JSON content string to validate
 * @param jsonLocation Location of the JSON file
 * @param jsonSchemaString JSON schema content string
 * @param publicJsonSchemaLocation Location of the public JSON schema
 */
CORE_EXPORT void validateJson(const std::string& jsonString, const std::string& jsonLocation, const std::string& jsonSchemaString, const std::string& publicJsonSchemaLocation);

}