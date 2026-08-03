// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "WorkflowExecutionContext.h"
#include "WorkflowPlan.h"

namespace mv::workflow
{

/**
 * @brief Executes workflow plans through the active runtime.
 *
 * This facade provides scoped access to the workflow runtime used by the
 * application. It keeps call sites independent from the concrete executor
 * implementation.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class CORE_EXPORT WorkflowRuntimeScoped
{
public:

    /**
     * @brief Returns the workflow runtime scope singleton.
     * @return Runtime scope instance.
     */
    [[nodiscard]] static WorkflowRuntimeScoped& instance();

    /**
     * @brief Executes a workflow plan synchronously under a parent context.
     * @param plan Workflow plan to execute.
     * @param parentContext Parent workflow execution context.
     * @return Final workflow result.
     */
    static SharedWorkflowResult executeBlocking(UniqueWorkflowPlan plan, const SharedWorkflowExecutionContext& parentContext);
};

}
