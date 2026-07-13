// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <workflow/WorkflowPlan.h>

/**
 * @brief Creates the workflow plan used to save a ManiVault project.
 *
 * The generated plan writes project, metadata, and workspace JSON files into a
 * temporary directory, archives them into the target project file, and performs
 * finalization once the save is complete.
 *
 * @param filePath Project file path to save to.
 * @return Workflow plan for saving the project.
 */
mv::workflow::UniqueWorkflowPlan createProjectSaveWorkflowPlan(const QString& filePath);
    
