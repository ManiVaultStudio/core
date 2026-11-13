// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2026 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include <ProjectResourceService.h>

#include <memory>

class QTemporaryDir;

namespace mv
{
class Project;
class ProjectManager;
class ProjectResourceStagingService;

std::unique_ptr<ProjectResourceService> createProjectResourceService(Project& project, ProjectManager& projectManager, std::shared_ptr<ProjectResourceStagingService> stagingService, std::shared_ptr<QTemporaryDir> stagingDirectory);
}
