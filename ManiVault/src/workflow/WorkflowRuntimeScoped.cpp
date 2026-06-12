// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowRuntimeScoped.h"
#include "Application.h"

namespace mv::workflow
{

WorkflowRuntimeScoped& WorkflowRuntimeScoped::instance()
{
    static WorkflowRuntimeScoped runtime;
    return runtime;
}

SharedWorkflowResult WorkflowRuntimeScoped::executeBlocking(UniqueWorkflowPlan plan, const SharedWorkflowExecutionContext& parentContext)
{
    if (!plan)
        return {};

    if (parentContext) {
        auto nestedContext = parentContext->createNestedWorkflowChild(plan->getName());

        return Application::getWorkflowPlanExecutor().executeBlocking(
            std::move(plan),
            nestedContext);
    }

    return Application::getWorkflowPlanExecutor().executeBlocking(std::move(plan));
}

}
