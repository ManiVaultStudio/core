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
    ArchiveBacked,
    OnDemand,
    BeforeLoad
};

[[nodiscard]] inline QString projectResourceStagingPolicyName(const ProjectResourceStagingPolicy policy)
{
    switch (policy)
    {
        case ProjectResourceStagingPolicy::Inherit: return QStringLiteral("Inherit");
        case ProjectResourceStagingPolicy::ArchiveBacked: return QStringLiteral("Archive-backed");
        case ProjectResourceStagingPolicy::OnDemand: return QStringLiteral("Stage on demand");
        case ProjectResourceStagingPolicy::BeforeLoad: return QStringLiteral("Stage before load");
    }

    return QStringLiteral("Inherit");
}

[[nodiscard]] inline ProjectResourceStagingPolicy projectResourceStagingPolicyFromName(const QString& name)
{
    if (name == QStringLiteral("Archive-backed")) return ProjectResourceStagingPolicy::ArchiveBacked;
    if (name == QStringLiteral("Stage on demand")) return ProjectResourceStagingPolicy::OnDemand;
    if (name == QStringLiteral("Stage before load")) return ProjectResourceStagingPolicy::BeforeLoad;
    return ProjectResourceStagingPolicy::Inherit;
}

}
