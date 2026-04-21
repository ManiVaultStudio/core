// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <util/AbstractWorkflowPlanExecutor.h>
#include <util/WorkflowPlan.h>
#include <util/WorkflowResult.h>

#include <QString>

namespace mv
{
    class Task;
}

/*
 * TaskTreeWorkflowPlanExecutor is a concrete implementation of the AbstractWorkflowPlanExecutor interface that executes a workflow plan using a task tree workflow. It creates a SerializePlanWorkflow with the provided workflow plan and starts it, waiting for the workflow to complete. If the workflow finishes with an error, it throws a runtime error with the error message. The workflow is stored as a unique pointer, which ensures that it is properly cleaned up when it is no longer needed. The workflow will be created in the execute method when the workflow plan is executed, and it will be reset to nullptr when the workflow is done.
 *
 * @author T. Kroes
 */
class WorkflowPlanExecutor final : public mv::util::AbstractWorkflowPlanExecutor
{
public:

	mv::util::WorkflowResult execute(mv::util::WorkflowPlan& workflowPlan, mv::Task* task = nullptr, ProgressCallback progressCallback = {}) override;

private:
    mv::util::WorkflowResult executeRoot(const mv::util::WorkflowPlan& workflowPlan) override;
    mv::util::WorkflowResult executeChild(const mv::util::WorkflowPlan& workflowPlan, mv::util::WorkflowExecutionContext& parentContext) override;
    void                     executeImpl(const mv::util::WorkflowPlan& workflowPlan) override;
    void                     executeStage(const mv::util::WorkflowPlan::Stage& stage, mv::util::WorkflowExecutionContext& stageContext) override;
    void executeSequentialJobs(const mv::util::WorkflowPlan::Stage& stage, mv::util::WorkflowExecutionContext& stageContext) override;
    void executeParallelJobs(const mv::util::WorkflowPlan::Stage& stage, mv::util::WorkflowExecutionContext& stageContext) override;
};

