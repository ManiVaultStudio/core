// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2026 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include <QString>

namespace mv::util
{

/** Staging behavior for archive-backed dataset resources during project open. */
enum class ProjectResourceStagingPolicy
{
    Inherit,
    ReadRangesOnDemand,
    StageOnFirstRead,
    StageImmediately
};

[[nodiscard]] inline QString projectResourceStagingPolicyName(const ProjectResourceStagingPolicy policy)
{
    switch (policy)
    {
        case ProjectResourceStagingPolicy::Inherit:
            return QStringLiteral("Inherit");

        case ProjectResourceStagingPolicy::ReadRangesOnDemand:
            return QStringLiteral("Read ranges on demand");

        case ProjectResourceStagingPolicy::StageOnFirstRead:
            return QStringLiteral("Stage on first read");

        case ProjectResourceStagingPolicy::StageImmediately:
            return QStringLiteral("Stage immediately");
    }

    return QStringLiteral("Inherit");
}

[[nodiscard]] inline ProjectResourceStagingPolicy projectResourceStagingPolicyFromName(const QString& name)
{
    if (name == QStringLiteral("Read ranges on demand") || name == QStringLiteral("Archive-backed"))
        return ProjectResourceStagingPolicy::ReadRangesOnDemand;
    if (name == QStringLiteral("Stage on first read") || name == QStringLiteral("Stage on demand"))
        return ProjectResourceStagingPolicy::StageOnFirstRead;
    if (name == QStringLiteral("Stage immediately") || name == QStringLiteral("Stage before load"))
        return ProjectResourceStagingPolicy::StageImmediately;

    return ProjectResourceStagingPolicy::Inherit;
}

[[nodiscard]] inline QString normalizeProjectResourceStagingPolicyName(const QString& name)
{
    if (name == QStringLiteral("Archive-backed") || name == QStringLiteral("Stage on demand") || name == QStringLiteral("Stage before load"))
        return projectResourceStagingPolicyName(projectResourceStagingPolicyFromName(name));

    return name;
}

}
