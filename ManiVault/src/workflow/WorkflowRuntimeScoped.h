// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "WorkflowExecutionContext.h"
#include "WorkflowPlan.h"

namespace mv::workflow
{

/**
 * @brief Access point for executing workflow plans in the current runtime scope.
 */
class CORE_EXPORT WorkflowRuntimeScoped
{
public:

    /** @return Singleton workflow runtime scope. */
    static WorkflowRuntimeScoped& instance();

    /**
     * @brief Executes a workflow plan synchronously under a parent context.
     * @param plan Workflow plan to execute.
     * @param parentContext Parent workflow execution context.
     * @return Final workflow result.
     */
    static SharedWorkflowResult executeBlocking(UniqueWorkflowPlan plan, const SharedWorkflowExecutionContext& parentContext);
};

}
