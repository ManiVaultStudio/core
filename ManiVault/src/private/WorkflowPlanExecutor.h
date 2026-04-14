// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <util/AbstractWorkflowPlanExecutor.h>
#include <util/Workflow.h>
#include <util/WorkflowPlan.h>

#include <QString>

/*
 * TaskTreeWorkflowPlanExecutor is a concrete implementation of the AbstractWorkflowPlanExecutor interface that executes a workflow plan using a task tree workflow. It creates a SerializePlanWorkflow with the provided workflow plan and starts it, waiting for the workflow to complete. If the workflow finishes with an error, it throws a runtime error with the error message. The workflow is stored as a unique pointer, which ensures that it is properly cleaned up when it is no longer needed. The workflow will be created in the execute method when the workflow plan is executed, and it will be reset to nullptr when the workflow is done.
 *
 * @author T. Kroes
 */
class WorkflowPlanExecutor final : public mv::util::AbstractWorkflowPlanExecutor
{
public:

	/**
     * Executes the given workflow plan by creating a SerializePlanWorkflow with the provided workflow plan and starting it. The method will wait for the workflow to complete and will throw a runtime error if the workflow finished with an error. The workflow is stored as a unique pointer, which ensures that it is properly cleaned up when it is no longer needed. The workflow will be created in this method when the workflow plan is executed, and it will be reset to nullptr when the workflow is done.
     * @param workflowPlan The workflow plan to execute, which contains the stages and jobs that define the workflow. The workflow plan will be passed to the SerializePlanWorkflow, which will execute the stages and jobs defined in the plan. The workflow plan can also contain shared state that can be accessed by the jobs in the workflow, allowing for better communication and data sharing between the jobs in the workflow.
	 */
	void execute(mv::util::WorkflowPlan& workflowPlan) override;

private:
    mv::util::UniqueWorkflow    _workflow;  /** The workflow that is currently running, used for storing the workflow instance while it is running. This allows the workflow to be canceled if needed, and it also allows for better control over the lifecycle of the workflow. The workflow is stored as a unique pointer, which ensures that it is properly cleaned up when it is no longer needed. The workflow will be created in the execute method when the workflow plan is executed, and it will be reset to nullptr when the workflow is done. */
};

