// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <nlohmann/json.hpp>

namespace mv::util {

/**
 * Load JSON from a resource file
 * @param resourcePath Path to the JSON resource file
 * @return Parsed JSON object
 */
CORE_EXPORT nlohmann::json loadJsonFromResource(const std::string& resourcePath);

/**
 * Validate JSON content against a schema defined in a resource file
 * @param json JSON content
 * @param jsonLocation Location of the JSON file
 * @param jsonSchemaResourcePath Path to the JSON resource file
 * @param publicJsonSchemaLocation Location of the public JSON schema
 */
CORE_EXPORT void validateJsonWithResourceSchema(const nlohmann::json& json, const std::string& jsonLocation, const std::string& jsonSchemaResourcePath, const std::string& publicJsonSchemaLocation);

}