// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowContextBase.h"

#include <QString>

#include <qtasktree.h>

/*
 * WorkflowRuntimeContext is a struct that holds the workflow context and an error message for the workflow. The workflow context is used for passing data between tasks in the workflow, while the error message can be used for storing any error that occurs during the workflow execution. The WorkflowRuntimeContext struct is used as the storage struct for the WorkflowRuntimeContextStorage, which is a QtTaskTree storage that holds the runtime context for the workflow.
 *
 * @author T. Kroes
 */
struct CORE_EXPORT WorkflowRuntimeContext
{
    UniqueWorkflowContext   _workflowContext;   /** Workflow context for passing data between tasks in the workflow. This is a unique pointer to a WorkflowContextBase, which can be used for storing any necessary information for the workflow that needs to be accessed by the tasks in the workflow. The workflow context will be initialized before the recipe is created and finalized after the recipe is done, allowing for better control over when the context is initialized and finalized. The workflow context can be used by tasks in the recipe to access any necessary information for their execution, and it can also be used by derived classes of AbstractWorkflow to implement custom logic in the setup and finalize methods based on the information stored in the context. */
    QString                 _errorMessage;      /** Error message for storing any error that occurs during the workflow execution. This can be used for logging or displaying error information to the user. */
    bool                    _success = false;   /** Success flag for indicating whether the workflow finished successfully or with an error. This can be used for logging or displaying success information to the user, and it can also be used by derived classes of AbstractWorkflow to implement custom logic in the setup and finalize methods based on whether the workflow finished successfully or with an error. */
};

/** WorkflowRuntimeContextStorage is a QtTaskTree storage that holds the runtime context for the workflow. It is used for passing data between tasks in the workflow, and it allows tasks to access the workflow context and any error message stored in it. The WorkflowRuntimeContextStorage is used as the storage for the workflow runtime context, and it can be accessed by tasks in the recipe using the contextStorage method of AbstractWorkflow. The WorkflowRuntimeContextStorage allows for better control over when the context is initialized and finalized, which can be useful for workflows that need to perform some setup before the recipe is created or need to perform some finalization after the recipe is done. */
using WorkflowRuntimeContextStorage = QtTaskTree::Storage<WorkflowRuntimeContext>;