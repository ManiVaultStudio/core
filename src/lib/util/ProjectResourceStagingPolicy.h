// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2026 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include <QString>

namespace mv::util
{

/**
 * @brief Controls when project-archive resources are copied to local staging.
 *
 * The policy is selected for a project at open time and applies to its
 * archive-backed dataset resources. It trades archive range reads and lazy
 * I/O for local-file availability; it does not change resource serialization.
 *
 * @author Thomas Kroes
 */
enum class ProjectResourceStagingPolicy
{
    Inherit,             /**< Use the project policy, falling back to the application default. */
    ReadRangesOnDemand,  /**< Keep the resource in the archive and read requested encoded ranges as needed. */
    StageOnFirstRead,    /**< Copy the complete resource to session storage when it is first accessed. */
    StageImmediately     /**< Copy the complete resource to session storage before project loading proceeds. */
};

/**
 * @brief Resolves dataset, project, and application staging policy overrides.
 * @param datasetPolicy Policy selected for the dataset
 * @param projectPolicy Project override, or Inherit to use the application default
 * @param applicationDefault Application-wide policy, or Inherit to use the safe on-demand fallback
 * @return The effective policy after applying dataset, project, and application precedence
 */
[[nodiscard]] inline ProjectResourceStagingPolicy resolveProjectResourceStagingPolicy(const ProjectResourceStagingPolicy datasetPolicy, const ProjectResourceStagingPolicy projectPolicy, const ProjectResourceStagingPolicy applicationDefault)
{
    if (datasetPolicy != ProjectResourceStagingPolicy::Inherit)
        return datasetPolicy;

    if (projectPolicy != ProjectResourceStagingPolicy::Inherit)
        return projectPolicy;

    if (applicationDefault != ProjectResourceStagingPolicy::Inherit)
        return applicationDefault;

    return ProjectResourceStagingPolicy::ReadRangesOnDemand;
}

/** Returns the canonical user-facing and serialized name of a policy. */
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

/**
 * Parses a policy name, accepting legacy names used by older project settings.
 * Unknown names resolve to Inherit so callers retain the normal fallback behavior.
 */
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

/** Converts recognized legacy policy names to their current canonical name. */
[[nodiscard]] inline QString normalizeProjectResourceStagingPolicyName(const QString& name)
{
    if (name == QStringLiteral("Archive-backed") || name == QStringLiteral("Stage on demand") || name == QStringLiteral("Stage before load"))
        return projectResourceStagingPolicyName(projectResourceStagingPolicyFromName(name));

    return name;
}

}
