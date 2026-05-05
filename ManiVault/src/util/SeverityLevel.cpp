// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SeverityLevel.h"

namespace mv::util
{

QString getSeverityLevelName(SeverityLevel level)
{
    if (const auto it = severityLevelNames.find(level); it != severityLevelNames.end()) {
        return it.value();
    }

    return "Unknown";
}

SeverityLevel getSeverityLevel(const QString& levelName)
{
    if (levelName.isEmpty())
        return SeverityLevel::Info;

    for (auto it = severityLevelNames.cbegin(); it != severityLevelNames.cend(); ++it) {
        if (it.value().compare(levelName, Qt::CaseInsensitive) == 0)
            return it.key();
    }

    return SeverityLevel::Info;
}

}
