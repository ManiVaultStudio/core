// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2026 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "ProjectResourceHandle.h"
#include "ProjectResourceReference.h"
#include "util/ProjectResourceStagingPolicy.h"

#include <QString>

#include <memory>

class QIODevice;

namespace mv
{
/**
 * @brief Project-owned contract for adding and reading file-backed resources.
 *
 * Resource names and extensions are opaque to this service. It stores resource
 * bytes in the project's save staging area and resolves references against the
 * owning project's archive. Plugins remain responsible for resource metadata
 * and serialization format.
 *
 * The service is owned by Project. Its archive-backed implementation belongs
 * to the application layer so the Core project API does not depend on a ZIP
 * library. The project manager provides convenience methods for the active
 * project's service and reports an error when no project is open.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ProjectResourceService
{
public:
    virtual ~ProjectResourceService();

    /**
     * Adds bytes to the active project's save staging area.
     *
     * @param source Open readable device. It is read from its current position.
     * @param resourceName Suggested archive name; the service assigns a unique
     * project-relative path and treats the extension as opaque.
     * @return Stable reference to the resource in the project archive.
     * @throws std::invalid_argument If the source is not readable or the name is empty.
     * @throws std::runtime_error If no save staging directory is available or copying fails.
     */
    virtual ProjectResourceReference addResource(QIODevice& source, const QString& resourceName) = 0;

    /** Adds a local file to the project using its file name unless a resource name is supplied. */
    virtual ProjectResourceReference addResourceFile(const QString& sourceFilePath, const QString& resourceName = {}) = 0;

    /** Opens a project resource using the requested policy; Inherit resolves through the active project's effective policy. */
    virtual std::shared_ptr<ProjectResourceHandle> openResource(const ProjectResourceReference& reference, util::ProjectResourceStagingPolicy stagingPolicy = util::ProjectResourceStagingPolicy::Inherit) const = 0;
};
}
