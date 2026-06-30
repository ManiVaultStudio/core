// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectPublishWorkflowPlan.h"
#include "ProjectPublishContext.h"
#include "ProjectManager.h"
#include "Archiver.h"

using namespace mv;
using namespace mv::gui;
using namespace mv::util;
using namespace mv::workflow;

#ifdef _DEBUG
	#define PROJECT_PUBLISH_WORKFLOW_PLAN_VERBOSE
#endif

UniqueWorkflowPlan createProjectPublishWorkflowPlan(const QString& filePath)
{
    auto context = std::make_shared<ProjectPublishContext>(filePath);

    UniqueWorkflowPlan plan = std::make_unique<WorkflowPlan>("Publish project workflow", context);

    plan->addSequentialStage("Setup", []() -> void {
#ifdef PROJECT_PUBLISH_WORKFLOW_PLAN_VERBOSE
        printLine("Recipe stage", "Setup", 2);
#endif
    });

    plan->addSequentialStage("Extract project archive", []() -> void {
#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
    printLine("Recipe stage", "Extract project archive", 2);
#endif
    });

    plan->addSequentialStage("Open project JSON", []() -> void {
#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
        printLine("Recipe stage", "Open project JSON", 2);
#endif
    });

    plan->addSequentialStage("Open workspace JSON", []() -> void {
#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
        printLine("Recipe stage", "Open workspace JSON", 2);
#endif
    });

    plan->addSequentialStage("Finalize", []() -> void {
#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
        printLine("Recipe stage", "Finalize", 2);
#endif
    });
    
    return plan;
}
