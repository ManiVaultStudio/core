// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <workflow/AbstractWorkflowPlanExecutor.h>
#include <workflow/WorkflowPlan.h>
#include <workflow/WorkflowResult.h>

#include <exception/ManiVaultException.h>

namespace mv
{
    class Task;
}

/**
 * @brief Legacy workflow plan executor implementation.
 *
 * WorkflowPlanExecutor implements AbstractWorkflowPlanExecutor without the
 * Taskflow graph backend. It executes workflow plans through the classic
 * executor path and remains available for code paths that do not use the
 * TaskflowWorkflowPlanExecutor.
 *
 * @maintainer T. Kroes
 */
class WorkflowPlanExecutor final : public mv::workflow::AbstractWorkflowPlanExecutor
{
public:

    /**
     * @brief Constructs a workflow plan executor.
     * @param parent Optional QObject parent.
     */
    WorkflowPlanExecutor(QObject* parent = nullptr);

    /**
     * @brief Executes a workflow asynchronously.
     * @param workflowPlan Workflow plan to execute.
     * @param parentContext Optional parent execution context.
     * @param options Optional workflow execution options.
     * @return Future representing the asynchronous workflow result.
     */
    mv::workflow::WorkflowResultFuture execute(mv::workflow::UniqueWorkflowPlan workflowPlan, mv::workflow::SharedWorkflowExecutionContext parentContext = nullptr, mv::workflow::OptionalWorkflowOptions options = std::nullopt) override;

protected:

    /**
     * @brief Implements asynchronous workflow execution.
     * @param workflowPlan Workflow plan to execute.
     * @param guiScope GUI scope associated with the backing task.
     * @param options Workflow execution options.
     * @param executionContext Execution context for the workflow.
     * @return Future representing the asynchronous workflow result.
     */
    mv::workflow::WorkflowResultFuture executeAsyncImpl(mv::workflow::UniqueWorkflowPlan workflowPlan, mv::Task::GuiScope guiScope, const mv::workflow::WorkflowOptions& options, mv::workflow::SharedWorkflowExecutionContext executionContext) override;

private:

    /**
     * @brief Executes a workflow as a root workflow.
     * @param workflowPlan Workflow plan to execute.
     * @param task Optional task used for progress reporting.
     * @param executionOptions Workflow execution options.
     * @return Workflow result.
     */
    mv::workflow::SharedWorkflowResult executeRoot(mv::workflow::WorkflowPlan& workflowPlan, mv::Task* task, const mv::workflow::WorkflowOptions& options = {}) override;

    /**
     * @brief Executes a workflow as a child workflow.
     * @param workflowPlan Workflow plan to execute.
     * @param parentContext Parent execution context.
     * @return Workflow result.
     */
    mv::workflow::SharedWorkflowResult executeChild(mv::workflow::WorkflowPlan& workflowPlan, mv::workflow::SharedWorkflowExecutionContext parentContext) override;

private: // Execute individual jobs

    /**
     * @brief Executes a workflow job on the GUI thread.
     * @param job Job to execute.
     * @param jobContext Execution context for the job.
     */
	void executeJobOnGuiThread(const mv::workflow::WorkflowPlan::Job& job, mv::workflow::SharedWorkflowExecutionContext jobContext) override;

    /**
     * @brief Executes a workflow job on a worker thread.
     * @param job Job to execute.
     * @param jobContext Execution context for the job.
     */
	void executeJobOnWorkerThread(const mv::workflow::WorkflowPlan::Job& job, mv::workflow::SharedWorkflowExecutionContext jobContext) override;

    /**
     * @brief Executes a workflow job using the appropriate thread.
     * @param job Job to execute.
     * @param jobContext Execution context for the job.
     */
	void executeJob(const mv::workflow::WorkflowPlan::Job& job, mv::workflow::SharedWorkflowExecutionContext jobContext) override;

private: // Helpers

    /**
     * @brief Reports a stage exception to the supplied stage context.
     * @param stage Stage that failed.
     * @param exception Exception raised by the stage.
     * @param stageContext Execution context for the failed stage.
     */
    static void handleStageException(const mv::workflow::WorkflowPlan::Stage& stage, const mv::ManiVaultException& exception, mv::workflow::SharedWorkflowExecutionContext stageContext);

};

