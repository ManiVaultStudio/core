// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QString>

#include <nlohmann/json.hpp>

namespace mv::util {

nlohmann::json loadJsonFromResource(const QString& resourcePath);

}