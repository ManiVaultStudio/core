// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "JSON.h"

using nlohmann::json;

namespace mv::util {

json loadJsonFromResource(const QString& resourcePath) {
    QFile file(resourcePath);

    if (!file.open(QIODevice::ReadOnly)) {
        throw std::runtime_error("Failed to open resource: " + resourcePath.toStdString());
    }

    const auto data = file.readAll();

	QJsonParseError error;

	const auto doc = QJsonDocument::fromJson(data, &error);

	if (error.error != QJsonParseError::NoError) {
        throw std::runtime_error("JSON parse error: " + error.errorString().toStdString());
    }

    return json::parse(doc.toJson(QJsonDocument::Compact).toStdString());
}

}
