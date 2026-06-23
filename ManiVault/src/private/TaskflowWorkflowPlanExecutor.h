// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "Taskflow.h"

#include <workflow/AbstractWorkflowPlanExecutor.h>
#include <workflow/WorkflowPlan.h>
#include <workflow/WorkflowResult.h>

#include <exception/ManiVaultException.h>

#include <QString>

namespace mv
{
    class Task;
}

using namespace mv::workflow;

/**
 * @brief Workflow executor implementation based on Taskflow.
 *
 * TaskflowWorkflowPlanExecutor compiles WorkflowPlan instances into Taskflow task
 * graphs and executes them on a shared Taskflow executor. Workflow stages are
 * translated into Taskflow tasks while preserving sequential and parallel
 * execution semantics, nested workflow execution, progress reporting, output
 * propagation, exception handling, and execution tracing.
 *
 * During compilation, workflow stages are converted into Taskflow tasks and
 * wrapped with lifecycle reporting nodes that measure execution time and report
 * stage progress. Nested workflows are compiled recursively into Taskflow
 * subflows, allowing complex workflow hierarchies to execute as a single task
 * graph while maintaining independent execution contexts.
 *
 * This executor is the primary runtime implementation of
 * AbstractWorkflowPlanExecutor and is responsible for transforming declarative
 * workflow plans into executable Taskflow graphs.
 *
 * @authors Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class TaskflowWorkflowPlanExecutor final : public AbstractWorkflowPlanExecutor
{
protected:
    using TaskList = std::vector<tf::Task>;

    struct CompiledTasks
    {
        TaskList starts;
        TaskList ends;
    };

public:

    TaskflowWorkflowPlanExecutor(QObject* parent = nullptr);

    /**
     * @brief Executes a workflow asynchronously.
     *
     * Creates or reuses an execution context, starts workflow execution, and returns
     * a future that resolves when the workflow has finished.
     *
     * @param workflowPlan Workflow plan to execute.
     * @param parentContext Optional parent execution context for nested execution.
     * @param executionOptions Optional workflow execution options.
     * @return Future representing the asynchronous workflow result.
     */
    WorkflowResultFuture execute(UniqueWorkflowPlan workflowPlan, SharedWorkflowExecutionContext parentContext = nullptr, OptionalWorkflowExecutionOptions executionOptions = std::nullopt) override;

    /**
     * @brief Executes a workflow synchronously as a root workflow.
     *
     * Blocks until the workflow has completed and returns the resulting workflow
     * status, messages, outputs, and errors.
     *
     * @param workflowPlan Workflow plan to execute.
     * @param task Optional task used for progress reporting.
     * @param executionOptions Workflow execution options.
     * @return Workflow result.
     */
    SharedWorkflowResult executeBlocking(UniqueWorkflowPlan workflowPlan, mv::Task* task = nullptr, WorkflowExecutionOptions executionOptions = {}) override;

    /**
	 * @brief Executes a workflow synchronously using an existing parent context.
	 *
	 * Runs the workflow as a child workflow within the supplied execution context.
	 *
	 * @param workflowPlan Workflow plan to execute.
	 * @param parentContext Parent execution context.
	 * @return Workflow result.
	 */
    SharedWorkflowResult executeBlocking(UniqueWorkflowPlan workflowPlan, SharedWorkflowExecutionContext parentContext) override;

protected:

    /**
	 * @brief Implements asynchronous workflow execution.
	 *
	 * Creates the runtime state needed for asynchronous execution and schedules the
	 * workflow while preserving GUI ownership and execution options.
	 *
	 * @param workflowPlan Workflow plan to execute.
	 * @param guiScope GUI scope associated with the backing task.
	 * @param executionOptions Workflow execution options.
	 * @param executionContext Execution context for the workflow.
	 * @return Future representing the asynchronous workflow result.
	 */
    WorkflowResultFuture executeAsyncImpl(UniqueWorkflowPlan workflowPlan, mv::Task::GuiScope guiScope, const WorkflowExecutionOptions& executionOptions, SharedWorkflowExecutionContext executionContext) override;

    /**
	 * @brief Executes a workflow as the root workflow.
	 *
	 * Sets up root workflow execution, progress reporting, notifications, tracing,
	 * and result collection.
	 *
	 * @param workflowPlan Workflow plan to execute.
	 * @param task Optional task used for progress reporting.
	 * @param executionOptions Workflow execution options.
	 * @return Workflow result.
	 */
    SharedWorkflowResult executeRoot(WorkflowPlan& workflowPlan, mv::Task* task, const WorkflowExecutionOptions& executionOptions = {}) override;

    /**
	 * @brief Executes a workflow using an existing root context.
	 *
	 * Compiles the workflow to a Taskflow graph, executes it, and collects the
	 * result from the supplied root context.
	 *
	 * @param workflowPlan Workflow plan to execute.
	 * @param rootContext Root execution context.
	 * @return Workflow result.
	 */
    SharedWorkflowResult executeWithContext(WorkflowPlan& workflowPlan, SharedWorkflowExecutionContext rootContext);

    /**
	 * @brief Executes a workflow as a child workflow.
	 *
	 * Runs the workflow inside an existing workflow hierarchy and reports its
	 * lifecycle through the parent execution context.
	 *
	 * @param workflowPlan Workflow plan to execute.
	 * @param parentContext Parent execution context.
	 * @return Workflow result.
	 */
    SharedWorkflowResult executeChild(WorkflowPlan& workflowPlan, SharedWorkflowExecutionContext parentContext) override;

private: // Execute individual jobs

    /**
	 * @brief Executes a workflow job on the GUI thread.
	 *
	 * Dispatches the job to the application thread so that GUI objects can be
	 * accessed safely.
	 *
	 * @param job Job to execute.
	 * @param jobContext Execution context for the job.
	 */
	void executeJobOnGuiThread(const WorkflowPlan::Job& job, SharedWorkflowExecutionContext jobContext) override;

    /**
	 * @brief Executes a workflow job on a worker thread.
	 *
	 * Runs the job directly on the current Taskflow worker thread.
	 *
	 * @param job Job to execute.
	 * @param jobContext Execution context for the job.
	 */
	void executeJobOnWorkerThread(const WorkflowPlan::Job& job, SharedWorkflowExecutionContext jobContext) override;

    /**
	 * @brief Executes a workflow job.
	 *
	 * Applies job lifecycle reporting, timing, exception handling, and output
	 * propagation around the job body.
	 *
	 * @param job Job to execute.
	 * @param jobContext Execution context for the job.
	 */
	void executeJob(const WorkflowPlan::Job& job, SharedWorkflowExecutionContext jobContext) override;

private: // Helpers

    /**
	 * @brief Handles an exception thrown while executing a stage.
	 *
	 * Reports the failure to the stage context and records the exception in the
	 * workflow result state.
	 *
	 * @param stage Stage that failed.
	 * @param exception Exception that was thrown.
	 * @param stageContext Execution context for the stage.
	 */
    static void handleStageException(const WorkflowPlan::Stage& stage, const mv::ManiVaultException& exception, SharedWorkflowExecutionContext stageContext);

    /**
	 * @brief Compiles a workflow into a Taskflow graph.
	 *
	 * Translates the workflow's stages and jobs into Taskflow tasks and dependency
	 * edges.
	 *
	 * @param workflowPlan Workflow plan to compile.
	 * @param taskflow Taskflow graph to populate.
	 * @param parentContext Parent execution context.
	 * @return Start and end tasks of the compiled workflow.
	 */
    CompiledTasks compileWorkflow(const WorkflowPlan& workflowPlan, tf::Taskflow& taskflow, SharedWorkflowExecutionContext parentContext);

    /**
	 * @brief Compiles a workflow into a Taskflow subflow.
	 *
	 * Used for nested workflow execution inside an existing Taskflow task.
	 *
	 * @param workflowPlan Workflow plan to compile.
	 * @param subflow Taskflow subflow to populate.
	 * @param parentContext Parent execution context.
	 * @return Start and end tasks of the compiled workflow.
	 */
    CompiledTasks compileWorkflow(const WorkflowPlan& workflowPlan, tf::Subflow& subflow, SharedWorkflowExecutionContext parentContext);

    /**
	 * @brief Adds a workflow completion notification.
	 *
	 * Creates a user-facing notification that summarizes the completed workflow
	 * result.
	 *
	 * @param workflowName Name of the workflow.
	 * @param result Workflow result.
	 * @param resultId Unique identifier of the workflow result.
	 */
    void addWorkflowFinishedNotification(const QString& workflowName, const SharedWorkflowResult& result, const QUuid& resultId);

    /**
	 * @brief Executes a compiled workflow job inside a Taskflow subflow.
	 *
	 * Executes normal jobs directly and expands nested workflow jobs into the
	 * supplied subflow.
	 *
	 * @param job Workflow job to execute.
	 * @param subflow Taskflow subflow used for nested workflows.
	 * @param jobContext Execution context for the job.
	 */
    void executeCompiledJob(const WorkflowPlan::Job& job, tf::Subflow& subflow, SharedWorkflowExecutionContext jobContext);

    /**
	 * @brief Compiles a sequence of workflow stages.
	 *
	 * Compiles each stage and connects the end tasks of one stage to the start
	 * tasks of the next stage.
	 *
	 * @tparam Flow Taskflow graph or subflow type.
	 * @param stages Stages to compile.
	 * @param flow Target graph or subflow.
	 * @param parentContext Parent execution context.
	 * @return Start and end tasks of the compiled stage sequence.
	 */
    template<typename Flow>
    CompiledTasks compileStages(const WorkflowPlan::Stages& stages, Flow& flow, SharedWorkflowExecutionContext parentContext)
    {
        CompiledTasks previous;

        for (const auto& stage : stages) {
            auto current = compileStage(stage, flow, parentContext);

            for (auto& prevEnd : previous.ends)
                for (auto& currentStart : current.starts)
                    prevEnd.precede(currentStart);

            previous = std::move(current);
        }

        return previous;
    }

    /**
	 * @brief Compiles a sequential workflow stage.
	 *
	 * Creates Taskflow tasks for all jobs in the stage and connects them in
	 * execution order.
	 *
	 * @tparam Flow Taskflow graph or subflow type.
	 * @param stage Stage to compile.
	 * @param flow Target graph or subflow.
	 * @param stageContext Execution context for the stage.
	 * @return Start and end tasks of the compiled stage.
	 */
    template<typename Flow>
    CompiledTasks compileSequentialStage(const WorkflowPlan::Stage& stage, Flow& flow, SharedWorkflowExecutionContext stageContext)
    {
        CompiledTasks result;
        tf::Task previous;

        const bool collapseSingleJob = stage.getJobs().size() == 1 && stage.getJobs().front().getName() == stage.getName();

        for (const auto& job : stage.getJobs()) {
            auto jobContext = collapseSingleJob ? stageContext : stageContext->createJobChild(job.getName(), job.getWeight(), job.getProgressMode());

            auto task = flow.emplace([this, job, jobContext](tf::Subflow& subflow) mutable {
                executeCompiledJob(job, subflow, jobContext);
            });

            task.name(makeTraceName(job.isNestedWorkflow() ? "Nested" : "Job", collapseSingleJob ? stage.getName() : job.getName()));

            if (result.starts.empty())
                result.starts.push_back(task);

            if (!result.ends.empty())
                result.ends.back().precede(task);

            result.ends = { task };
        }

        return result;
    }

    /**
	 * @brief Compiles a parallel workflow stage.
	 *
	 * Creates independent Taskflow tasks for all jobs in the stage so they may run
	 * concurrently.
	 *
	 * @tparam Flow Taskflow graph or subflow type.
	 * @param stage Stage to compile.
	 * @param flow Target graph or subflow.
	 * @param stageContext Execution context for the stage.
	 * @return Start and end tasks of the compiled stage.
	 */
    template<typename Flow>
    CompiledTasks compileParallelStage(const WorkflowPlan::Stage& stage, Flow& flow, SharedWorkflowExecutionContext stageContext)
    {
        CompiledTasks result;

        const bool collapseSingleJob = stage.getJobs().size() == 1 && stage.getJobs().front().getName() == stage.getName();

        for (const auto& job : stage.getJobs()) {
            auto jobContext = collapseSingleJob ? stageContext : stageContext->createJobChild(job.getName(), job.getWeight(), job.getProgressMode());

            auto task = flow.emplace([this, job, jobContext](tf::Subflow& subflow) mutable {
                executeCompiledJob(job, subflow, jobContext);
            });

            task.name(makeTraceName(job.isNestedWorkflow() ? "Nested" : "Job", collapseSingleJob ? stage.getName() : job.getName()));

            result.starts.push_back(task);
            result.ends.push_back(task);
        }

        return result;
    }

    /**
	 * @brief Compiles a workflow stage.
	 *
	 * Creates the stage execution context, adds lifecycle reporting tasks, compiles
	 * the contained jobs, and connects the resulting tasks.
	 *
	 * @tparam Flow Taskflow graph or subflow type.
	 * @param stage Stage to compile.
	 * @param flow Target graph or subflow.
	 * @param parentContext Parent execution context.
	 * @return Start and end tasks of the compiled stage.
	 */
    template<typename Flow>
    CompiledTasks compileStage(const WorkflowPlan::Stage& stage, Flow& flow, SharedWorkflowExecutionContext parentContext)
    {
        const bool isSequential = stage.getConcurrencyMode() == WorkflowPlan::ConcurrencyMode::Sequential;

        auto stageContext = isSequential ? parentContext->createSequentialStageChild(stage.getName(), stage.getWeight(), WorkflowPlan::JobProgressMode::Nested) : parentContext->createParallelStageChild(stage.getName(), stage.getWeight(), WorkflowPlan::JobProgressMode::Nested);

        stageContext->setOutputId(stage.getId());

        auto timer = std::make_shared<QElapsedTimer>();

        auto startTask = flow.emplace([stageContext, timer]() {
            timer->start();
            stageContext->reportStarted();
        });

        startTask.name(makeTraceName("Stage begin", stage.getName()));

        auto compiled = isSequential ? compileSequentialStage(stage, flow, stageContext) : compileParallelStage(stage, flow, stageContext);

        auto finishTask = flow.emplace([stageContext, timer]() {
            stageContext->reportFinished(static_cast<std::uint64_t>(timer->elapsed()));
        });

        finishTask.name(makeTraceName("Stage end", stage.getName()));

        for (auto& start : compiled.starts)
            startTask.precede(start);

        for (auto& end : compiled.ends)
            end.precede(finishTask);

        return { { startTask }, { finishTask } };
    }

    /**
	 * @brief Returns the handle of the final stage output.
	 *
	 * Determines which workflow stage output should be published as the workflow's
	 * final output.
	 *
	 * @param workflowPlan Workflow plan to inspect.
	 * @return Handle of the final output stage.
	 */
    static WorkflowHandle getFinalStageHandle(const WorkflowPlan& workflowPlan);

    /**
	 * @brief Compiles a complete workflow implementation.
	 *
	 * Compiles main stages, success stages, dependency edges, and optional final
	 * workflow output publication into the supplied graph.
	 *
	 * @tparam Graph Taskflow graph or subflow type.
	 * @param workflowPlan Workflow plan to compile.
	 * @param graph Target graph or subflow.
	 * @param parentContext Parent execution context.
	 * @return Start and end tasks of the compiled workflow.
	 */
    template<typename Graph>
    CompiledTasks compileWorkflowImpl(const WorkflowPlan& workflowPlan, Graph& graph, SharedWorkflowExecutionContext parentContext)
    {
        auto mainTasks      = compileStages(workflowPlan.getStages(), graph, parentContext);
        auto successTasks   = compileStages(workflowPlan.getOnSuccessStages(), graph, parentContext);

        CompiledTasks result = mainTasks;

        if (!successTasks.starts.empty()) {
            for (auto& mainEnd : mainTasks.ends)
                for (auto& successStart : successTasks.starts)
                    mainEnd.precede(successStart);

            result = {
                mainTasks.starts,
                successTasks.ends
            };
        }

        const auto finalHandle = getFinalStageHandle(workflowPlan);

        if (finalHandle.isValid() && !result.ends.empty()) {
            auto publishWorkflowOutputTask = graph.emplace([parentContext, finalHandle]() {
                auto output = parentContext->takeOutput(finalHandle);

                if (output.isValid() && !output.isNull())
                    parentContext->setOutput(output);
            });

            publishWorkflowOutputTask.name(makeTraceName("Workflow output", workflowPlan.getName()));

            for (auto& end : result.ends)
                end.precede(publishWorkflowOutputTask);

            result.ends = { publishWorkflowOutputTask };
        }

        return result;
    }

    /**
	 * @brief Executes root workflow stages directly.
	 *
	 * Runs the supplied stages in the root execution context.
	 *
	 * @param stages Stages to execute.
	 * @param rootContext Root execution context.
	 */
    void runStagesRoot(const WorkflowPlan::Stages& stages, SharedWorkflowExecutionContext rootContext);

    /**
	 * @brief Runs a Taskflow graph and blocks until completion.
	 *
	 * Executes the graph on the internal Taskflow executor and waits for all tasks
	 * to finish.
	 *
	 * @param taskflow Taskflow graph to execute.
	 */
    void runTaskflowBlocking(tf::Taskflow& taskflow);

    /**
	 * @brief Creates a trace name for workflow profiling.
	 *
	 * Combines a trace event kind and workflow object name into a profiler-friendly
	 * string.
	 *
	 * @param kind Trace event kind.
	 * @param name Workflow object name.
	 * @return Trace name.
	 */
    static std::string makeTraceName(const QString& kind, const QString& name);

private:
    tf::Executor                            _executor;          /** Shared Taskflow executor for running workflow graphs */
    std::shared_ptr<tf::ChromeObserver>     _chromeObserver;    /** Chrome trace observer for profiling workflow execution */
};

