// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

namespace mv::util
{

class WorkflowExecutionContext;
using SharedWorkflowExecutionContext = std::shared_ptr<WorkflowExecutionContext>;

/**
 * @brief The WorkflowExecutionScope class manages the active workflow execution context within a specific scope. When an instance of WorkflowExecutionScope is created, it sets the provided WorkflowExecutionContext as the current context for the duration of the scope. When the scope is exited (e.g., when the WorkflowExecutionScope object is destroyed), it automatically restores the previous workflow execution context that was active before this scope was created. This ensures that the correct workflow execution context is maintained throughout the execution of jobs and stages within a workflow plan, and that any changes to the current context are properly scoped and do not affect other parts of the workflow execution.
 *
 * @author T. Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class CORE_EXPORT WorkflowExecutionScope
{
public:

    /**
     * @brief Constructs a WorkflowExecutionScope object that sets the provided WorkflowExecutionContext as the current context for the duration of the scope. The previous workflow execution context that was active before this scope was created is stored and will be restored when the scope is destroyed.
     * @param context The WorkflowExecutionContext to be set as the current context for the duration of this scope.
     */
    explicit WorkflowExecutionScope(SharedWorkflowExecutionContext context);

    /** @brief Destroys the WorkflowExecutionScope object and restores the previous workflow execution context that was active before this scope was created. This ensures that any changes to the current context made within this scope are properly scoped and do not affect other parts of the workflow execution. */
    ~WorkflowExecutionScope();

private:
    SharedWorkflowExecutionContext  _previous = nullptr;    /** Previous workflow execution context that was active before this scope was created. This is used to restore the previous context when the scope is destroyed. */
    SharedWorkflowExecutionContext  _context = nullptr;     /** The workflow execution context that is active within this scope. This context will be set as the current context for the duration of this scope, and will be restored to the previous context when the scope is destroyed. */
};

}
