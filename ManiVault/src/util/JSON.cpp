// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "JSON.h"

#include <nlohmann/json.hpp>
#include <valijson/adapters/nlohmann_json_adapter.hpp>
#include <valijson/validator.hpp>
#include <valijson/schema.hpp>
#include <valijson/schema_parser.hpp>
#include <valijson/utils/nlohmann_json_utils.hpp>
#include <valijson/validation_results.hpp>

#include <QString>

using nlohmann::json;

using valijson::Schema;
using valijson::SchemaParser;
using valijson::Validator;
using valijson::adapters::NlohmannJsonAdapter;

namespace mv::util {

bool isValidJson(const std::string& input)
{
	return nlohmann::json::accept(input);
}

std::string loadJsonFromResource(const std::string& resourcePath) {
    QFile file(QString::fromStdString(resourcePath));

    if (!file.open(QIODevice::ReadOnly)) {
        throw std::runtime_error("Failed to open resource: " + resourcePath);
    }

    return file.readAll().toStdString();
}

void validateJson(const std::string& jsonString, const std::string& jsonLocation, const std::string& jsonSchemaString, const std::string& publicJsonSchemaLocation /*= ""*/)
{
    try {
        if (jsonString.empty())
            throw std::runtime_error("JSON content is empty");

        if (jsonSchemaString.empty())
            throw std::runtime_error("JSON schema is empty");

        if (!isValidJson(jsonString))
            throw std::runtime_error("Input content is not properly JSON formatted");

        if (!isValidJson(jsonSchemaString))
            throw std::runtime_error("Schema content is not properly JSON formatted");

        nlohmann::json jsonDocument = nlohmann::json::parse(jsonString), jsonSchemaDocument = nlohmann::json::parse(jsonSchemaString);

        valijson::Schema schema;
        valijson::adapters::NlohmannJsonAdapter schemaAdapter(jsonSchemaDocument);

        valijson::SchemaParser parser;

        parser.populateSchema(schemaAdapter, schema);

        valijson::Validator validator;
        valijson::adapters::NlohmannJsonAdapter documentAdapter(jsonDocument);

        valijson::ValidationResults results;

        if (validator.validate(schema, documentAdapter, &results)) {
            qDebug() << "JSON document" << jsonLocation << "is valid";
        }
        else {
            qCritical() << "JSON document" << jsonLocation << "is invalid";

            valijson::ValidationResults::Error error;
            while (results.popError(error)) {
                std::cerr << "  - " << error.description;
                if (!error.context.empty()) {
                    std::cerr << " [Context:";
                    for (const auto& ctx : error.context) {
                        std::cerr << " " << ctx;
                    }
                    std::cerr << "]";
                }
                std::cerr << std::endl;
            }
        }
    }
    catch (const std::exception& e) {
        qCritical() << "Unable to validate JSON: " << e.what() << "\n";
    }
}

}
