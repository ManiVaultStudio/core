// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <workflow/WorkflowPlan.h>

/**
 * @brief Creates the workflow plan used to open a ManiVault project.
 *
 * The generated plan extracts project archive files, reads project/workspace
 * metadata, restores project state, restores workspace state, and performs
 * finalization on success or failure.
 *
 * @param filePath Project file path to open.
 * @return Workflow plan for opening the project.
 */
mv::workflow::UniqueWorkflowPlan createProjectOpenWorkflowPlan(const QString& filePath);
    
