// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "Taskflow.h"

#include <util/AbstractWorkflowPlanExecutor.h>
#include <util/WorkflowPlan.h>
#include <util/WorkflowResult.h>

#include <exception/ManiVaultException.h>

#include <QString>

namespace mv
{
    class Task;
}

/*
 *
 * @author T. Kroes
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

    WorkflowResultFuture execute(UniqueWorkflowPlan workflowPlan, SharedWorkflowExecutionContext parentContext = nullptr, OptionalWorkflowExecutionOptions executionOptions = std::nullopt) override;
    SharedWorkflowResult executeBlocking(UniqueWorkflowPlan workflowPlan, mv::Task* task = nullptr, WorkflowExecutionOptions executionOptions = {}) override;
    SharedWorkflowResult executeBlocking(UniqueWorkflowPlan workflowPlan, SharedWorkflowExecutionContext parentContext) override;

protected:
    WorkflowResultFuture executeAsyncImpl(UniqueWorkflowPlan workflowPlan, mv::Task::GuiScope guiScope, const WorkflowExecutionOptions& executionOptions, SharedWorkflowExecutionContext executionContext) override;
    SharedWorkflowResult executeRoot(WorkflowPlan& workflowPlan, mv::Task* task, const WorkflowExecutionOptions& executionOptions = {}) override;

    SharedWorkflowResult executeWithContext(WorkflowPlan& workflowPlan, SharedWorkflowExecutionContext rootContext);

    SharedWorkflowResult executeChild(WorkflowPlan& workflowPlan, SharedWorkflowExecutionContext parentContext) override;

private: // Execute individual jobs
	void executeJobOnGuiThread(const WorkflowPlan::Job& job, SharedWorkflowExecutionContext jobContext) override;
	void executeJobOnWorkerThread(const WorkflowPlan::Job& job, SharedWorkflowExecutionContext jobContext) override;
	void executeJob(const WorkflowPlan::Job& job, SharedWorkflowExecutionContext jobContext) override;

private: // Helpers

    static void handleStageException(const WorkflowPlan::Stage& stage, const mv::ManiVaultException& exception, SharedWorkflowExecutionContext stageContext);

    CompiledTasks compileWorkflow(
        const WorkflowPlan& workflowPlan,
        tf::Taskflow& taskflow,
        SharedWorkflowExecutionContext parentContext);

    CompiledTasks compileWorkflow(
        const WorkflowPlan& workflowPlan,
        tf::Subflow& subflow,
        SharedWorkflowExecutionContext parentContext);

    void addWorkflowFinishedNotification(const QString& workflowName, const SharedWorkflowResult& result, const QUuid& resultId);

    /**
	 * @brief Executes a compiled workflow job within a Taskflow subflow.
	 *
	 * If the job represents a nested workflow, this function creates the nested workflow plan,
	 * creates a nested workflow execution context, reports the nested workflow lifecycle, compiles
	 * the nested workflow into the supplied subflow, and waits for the subflow to complete.
	 *
	 * If the job is a normal executable job, it is executed directly through executeJob().
	 *
	 * @param job The workflow job to execute.
	 * @param subflow The Taskflow subflow used for nested workflow execution.
	 * @param jobContext Execution context associated with the compiled job.
	 */
    void executeCompiledJob(const WorkflowPlan::Job& job, tf::Subflow& subflow, SharedWorkflowExecutionContext jobContext);

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

    template<typename Flow>
    CompiledTasks compileSequentialStage(
        const WorkflowPlan::Stage& stage,
        Flow& flow,
        SharedWorkflowExecutionContext stageContext)
    {
        CompiledTasks result;
        tf::Task previous;

        const bool collapseSingleJob = stage.getJobs().size() == 1 && stage.getJobs().front().getName() == stage.getName();

        for (const auto& job : stage.getJobs()) {
            auto jobContext = collapseSingleJob ? stageContext : stageContext->createJobChild(job.getName(), job.getWeight(), job.getProgressMode());

            auto task = flow.emplace([this, job, jobContext](tf::Subflow& subflow) mutable {
                executeCompiledJob(job, subflow, jobContext);
            });

            if (result.starts.empty())
                result.starts.push_back(task);

            if (!result.ends.empty())
                result.ends.back().precede(task);

            result.ends = { task };
        }

        return result;
    }

    template<typename Flow>
    CompiledTasks compileParallelStage(
        const WorkflowPlan::Stage& stage,
        Flow& flow,
        SharedWorkflowExecutionContext stageContext)
    {
        CompiledTasks result;

        const bool collapseSingleJob = stage.getJobs().size() == 1 && stage.getJobs().front().getName() == stage.getName();

        for (const auto& job : stage.getJobs()) {
            auto jobContext = collapseSingleJob ? stageContext : stageContext->createJobChild(job.getName(), job.getWeight(), job.getProgressMode());

            auto task = flow.emplace([this, job, jobContext](tf::Subflow& subflow) mutable {
                executeCompiledJob(job, subflow, jobContext);
                });

            result.starts.push_back(task);
            result.ends.push_back(task);
        }

        return result;
    }

    /**
	 * @brief Compiles a workflow stage and wraps it with timed lifecycle reporting tasks.
	 *
	 * The generated Taskflow structure emits the stage start event at execution time, executes
	 * the compiled stage jobs, and then emits the stage finish event with the measured elapsed
	 * wall-clock duration.
	 *
	 * @tparam Flow Taskflow graph type.
	 * @param stage Stage to compile.
	 * @param flow Target Taskflow graph or subflow.
	 * @param parentContext Parent execution context.
	 * @return Start and end tasks for dependency chaining.
	 */
    template<typename Flow>
    CompiledTasks compileStage(
        const WorkflowPlan::Stage& stage,
        Flow& flow,
        SharedWorkflowExecutionContext parentContext)
    {
        const bool isSequential =
            stage.getConcurrencyMode() == WorkflowPlan::ConcurrencyMode::Sequential;

        auto stageContext = isSequential
            ? parentContext->createSequentialStageChild(stage.getName(), stage.getWeight(), WorkflowPlan::JobProgressMode::Nested)
            : parentContext->createParallelStageChild(stage.getName(), stage.getWeight(), WorkflowPlan::JobProgressMode::Nested);

        auto timer = std::make_shared<QElapsedTimer>();

        auto startTask = flow.emplace([stageContext, timer]() {
            timer->start();
            stageContext->reportStarted();
            });

        auto compiled = isSequential
            ? compileSequentialStage(stage, flow, stageContext)
            : compileParallelStage(stage, flow, stageContext);

        auto finishTask = flow.emplace([stageContext, timer]() {
            stageContext->reportFinished(static_cast<std::uint64_t>(timer->elapsed()));
            });

        for (auto& start : compiled.starts)
            startTask.precede(start);

        for (auto& end : compiled.ends)
            end.precede(finishTask);

        return { { startTask }, { finishTask } };
    }

    template<typename Graph>
    CompiledTasks compileWorkflowImpl(const WorkflowPlan& workflowPlan, Graph& graph, SharedWorkflowExecutionContext parentContext)
    {
        auto mainTasks      = compileStages(workflowPlan.getStages(), graph, parentContext);
        auto successTasks   = compileStages(workflowPlan.getOnSuccessStages(), graph, parentContext);

        if (!successTasks.starts.empty()) {
            for (auto& mainEnd : mainTasks.ends)
                for (auto& successStart : successTasks.starts)
                    mainEnd.precede(successStart);

            return {
                mainTasks.starts,
                successTasks.ends
            };
        }

        return mainTasks;
    }

    void runStagesRoot(
        const WorkflowPlan::Stages& stages,
        SharedWorkflowExecutionContext rootContext);

    void runTaskflowBlocking(tf::Taskflow& taskflow);

private:
    tf::Executor    _executor;  
};

