// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <util/WorkflowResultBase.h>

class ProjectSaveResult : public WorkflowResultBase
{
public:
    QString     _filePath;
};

using UniqueProjectSaveResult = std::unique_ptr<ProjectSaveResult>;