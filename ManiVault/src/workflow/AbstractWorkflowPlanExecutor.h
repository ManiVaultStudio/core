// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowPlan.h"
#include "WorkflowExecutionContext.h"
#include "WorkflowResult.h"
#include "WorkflowResultFuture.h"
#include "Task.h"

#include <QObject>

namespace mv::workflow
{

/**
 * @brief Abstract base class for workflow plan executors.
 *
 * A workflow plan executor is responsible for executing WorkflowPlan instances,
 * managing workflow execution contexts, dispatching jobs to the appropriate
 * execution thread, tracking progress, and producing WorkflowResult objects.
 *
 * Workflow plans can be executed either as root workflows or as nested child
 * workflows. Root workflows own their execution lifecycle and optionally create
 * tasks, notifications, and progress reporting infrastructure. Child workflows
 * execute within an existing WorkflowExecutionContext and contribute their
 * results to the parent workflow.
 *
 * Concrete implementations define the execution strategy, scheduling model,
 * threading behavior, and workflow compilation process.
 *
 * @see WorkflowPlan
 * @see WorkflowExecutionContext
 * @see WorkflowResult
 *
 * @author T. Kroes
 */
class CORE_EXPORT AbstractWorkflowPlanExecutor : public QObject
{
public:

    /**
     * Construct a workflow plan executor.
     *
     * @param parent Optional QObject parent.
     */
    AbstractWorkflowPlanExecutor(QObject* parent = nullptr);

    /**
     * Execute a workflow asynchronously.
     *
     * If no parent execution context is supplied, the workflow is executed as
     * a root workflow. Otherwise it is executed as a child workflow within the
     * supplied execution context.
     *
     * The returned future can be used to monitor execution progress and obtain
     * the final WorkflowResult when execution completes.
     *
     * @param workflowPlan Workflow plan to execute.
     * @param parentContext Optional parent workflow execution context.
     * @param executionOptions Optional execution configuration.
     * @return Future representing the workflow execution.
     */
    [[nodiscard]] virtual WorkflowResultFuture execute(UniqueWorkflowPlan workflowPlan, SharedWorkflowExecutionContext parentContext = nullptr, OptionalWorkflowExecutionOptions executionOptions = std::nullopt) = 0;

    /**
     * Execute a workflow synchronously as a root workflow.
     *
     * The calling thread is blocked until execution completes.
     *
     * Implementations may create a root execution context, progress task,
     * notifications, tracing infrastructure, and other root-level resources.
     *
     * @param workflowPlan Workflow plan to execute.
     * @param task Optional task associated with the workflow.
     * @param executionOptions Workflow execution options.
     * @return Final workflow result.
     */
    [[nodiscard]] virtual SharedWorkflowResult executeBlocking(UniqueWorkflowPlan workflowPlan, Task* task = nullptr, WorkflowExecutionOptions executionOptions = {}) = 0;

    /**
     * Execute a workflow synchronously as a child workflow.
     *
     * The workflow executes within the supplied parent execution context and
     * contributes its progress, outputs, and lifecycle events to the parent.
     *
     * @param workflowPlan Workflow plan to execute.
     * @param parentContext Parent workflow execution context.
     * @return Final workflow result.
     */
    [[nodiscard]] virtual SharedWorkflowResult executeBlocking(UniqueWorkflowPlan workflowPlan, SharedWorkflowExecutionContext parentContext) = 0;

    /**
     * Wait for completion of an asynchronous workflow execution.
     *
     * This helper blocks until the supplied asynchronous execution state has
     * completed and then returns the resulting workflow result.
     *
     * @param state Asynchronous workflow state.
     * @return Final workflow result.
     */
    static SharedWorkflowResult waitForAsync(WorkflowResultFuture::State& state);

    /**
     * Install the notification link handler used by workflow notifications.
     *
     * Implementations use this to register handlers that allow users to open
     * workflow results, logs, traces, or other workflow-related resources from
     * notifications.
     */
	static void installNotificationLinkHandler();

protected:

    /**
     * Execute a workflow asynchronously using an existing execution context.
     *
     * This method contains the implementation-specific asynchronous execution
     * logic and is typically used after the appropriate root or child context
     * has been created.
     *
     * @param workflowPlan Workflow plan to execute.
     * @param guiScope GUI execution scope associated with the workflow.
     * @param executionOptions Workflow execution options.
     * @param executionContext Execution context for the workflow.
     * @return Future representing workflow execution.
     */
    [[nodiscard]] virtual WorkflowResultFuture executeAsyncImpl(UniqueWorkflowPlan workflowPlan, Task::GuiScope guiScope, const WorkflowExecutionOptions& executionOptions, SharedWorkflowExecutionContext executionContext) = 0;

    /**
     * Execute a workflow as a root workflow.
     *
     * Root workflows own their execution lifecycle and are responsible for
     * creating the root workflow execution context.
     *
     * @param workflowPlan Workflow plan to execute.
     * @param task Optional task associated with the workflow.
     * @param executionOptions Workflow execution options.
     * @return Final workflow result.
     */
    [[nodiscard]] virtual SharedWorkflowResult executeRoot(WorkflowPlan& workflowPlan, Task* task, const WorkflowExecutionOptions& executionOptions = {}) = 0;

    /**
     * Execute a workflow as a nested child workflow.
     *
     * Child workflows execute within an existing workflow execution context
     * created by a parent workflow.
     *
     * @param workflowPlan Workflow plan to execute.
     * @param parentContext Parent workflow execution context.
     * @return Final workflow result.
     */
    [[nodiscard]] virtual SharedWorkflowResult executeChild(WorkflowPlan& workflowPlan, SharedWorkflowExecutionContext parentContext) = 0;

private: // Execute individual jobs

    /**
     * Execute a workflow job on the GUI thread.
     *
     * This method is used for jobs that require GUI thread affinity.
     *
     * @param job Job to execute.
     * @param jobContext Execution context associated with the job.
     */
    virtual void executeJobOnGuiThread(const WorkflowPlan::Job& job, SharedWorkflowExecutionContext jobContext) = 0;

    /**
     * Execute a workflow job on a worker thread.
     *
     * This method is used for jobs that can safely execute outside the GUI
     * thread.
     *
     * @param job Job to execute.
     * @param jobContext Execution context associated with the job.
     */
    virtual void executeJobOnWorkerThread(const WorkflowPlan::Job& job, SharedWorkflowExecutionContext jobContext) = 0;

    /**
     * Execute a workflow job using the appropriate execution thread.
     *
     * Implementations typically dispatch to either executeJobOnGuiThread()
     * or executeJobOnWorkerThread() depending on the job requirements.
     *
     * @param job Job to execute.
     * @param jobContext Execution context associated with the job.
     */
    virtual void executeJob(const WorkflowPlan::Job& job, SharedWorkflowExecutionContext jobContext) = 0;

protected:

    /**
     * Validate that a workflow execution context exists.
     *
     * Throws if the supplied context is null.
     *
     * @param context Context to validate.
     * @param where Name of the calling function for diagnostics.
     * @return Valid execution context.
     */
    static SharedWorkflowExecutionContext requireContext(const SharedWorkflowExecutionContext& context, const char* where);

private:
    friend class WorkflowPlan;
};

using SharedWorkflowPlanExecutor = std::shared_ptr<AbstractWorkflowPlanExecutor>;
using UniqueWorkflowPlanExecutor = std::unique_ptr<AbstractWorkflowPlanExecutor>;

} 