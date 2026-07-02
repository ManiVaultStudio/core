// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "WorkflowExecutionContext.h"
#include "WorkflowPlan.h"

namespace mv::workflow
{

/**
 * @brief Scoped access point for executing workflow plans through the application runtime.
 *
 * WorkflowRuntimeScoped is a lightweight singleton facade over the application's
 * workflow plan executor. It provides a blocking execution helper that can run a
 * workflow plan either as a root execution or as a nested workflow beneath an
 * existing WorkflowExecutionContext.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class CORE_EXPORT WorkflowRuntimeScoped
{
public:

    /**
     * @brief Returns the global scoped workflow runtime facade.
     * @return Singleton runtime facade instance.
     */
    [[nodiscard]] static WorkflowRuntimeScoped& instance();

    /**
     * @brief Executes a workflow plan synchronously.
     *
     * When a parent context is provided, the plan is executed as a nested
     * workflow child of that context. Otherwise, it is executed as a root
     * workflow plan. Passing a null plan returns an empty result pointer.
     *
     * @param plan Workflow plan to execute.
     * @param parentContext Optional parent execution context.
     * @return Workflow result produced by the execution, or nullptr for a null plan.
     */
    static SharedWorkflowResult executeBlocking(UniqueWorkflowPlan plan, const SharedWorkflowExecutionContext& parentContext);
};

}
