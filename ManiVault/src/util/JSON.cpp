// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "JSON.h"

#include <regex>

using nlohmann::json;
using namespace nlohmann::json_schema;
using nlohmann::json_schema::json_validator;

namespace mv::util {

json loadJsonFromResource(const QString& resourcePath) {
    QFile file(resourcePath);

    if (!file.open(QIODevice::ReadOnly)) {
        throw std::runtime_error("Failed to open resource: " + resourcePath.toStdString());
    }

    QByteArray data = file.readAll();

    return nlohmann::json::parse(data.constData());
}

bool jsonIsUri(const std::string& value) {
    static const std::regex uri_regex(R"(^https?://)");
    return std::regex_search(value, uri_regex);
}

bool jsonIsDateTime(const std::string& value) {
    static const std::regex datetime_regex(R"(^\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}Z$)");
    return std::regex_match(value, datetime_regex);
}

static void my_format_checker(const std::string& format, const std::string& value)
{
    //if (format == "something") {
    //    if (!check_value_for_something(value))
    //        throw std::invalid_argument("value is not a good something");
    //}
    //else
    //    throw std::logic_error("Don't know how to validate " + format);
}

void validateJsonWithResourceSchema(const QString& json, const QString& resourcePath)
{
    const auto jsonSchema = loadJsonFromResource(resourcePath);

    //qDebug() << jsonSchema;

    json_validator validator(nullptr, my_format_checker);

    validator.set_root_schema(jsonSchema);

    JsonSchemaErrorHandler jsonSchemaErrorHandler;

    validator.validate(json.toStdString(), jsonSchemaErrorHandler);

    //if (!validator.validate(jsonData))
    //    throw std::runtime_error("Invalid JSON schema for learning center tutorials.");
}

}
