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

protected:
    WorkflowResultFuture executeAsyncImpl(UniqueWorkflowPlan workflowPlan, mv::Task::GuiScope guiScope, const WorkflowExecutionOptions& executionOptions, SharedWorkflowExecutionContext executionContext) override;
    SharedWorkflowResult executeRoot(WorkflowPlan& workflowPlan, mv::Task* task, const WorkflowExecutionOptions& executionOptions = {}) override;
    SharedWorkflowResult executeChild(WorkflowPlan& workflowPlan, SharedWorkflowExecutionContext parentContext) override;

private: // Execute individual jobs
	void executeJobOnGuiThread(const WorkflowPlan::Job& job, SharedWorkflowExecutionContext jobContext) override;
	void executeJobOnWorkerThread(const WorkflowPlan::Job& job, SharedWorkflowExecutionContext jobContext) override;
	void executeJob(const WorkflowPlan::Job& job, SharedWorkflowExecutionContext jobContext) override;

private: // Helpers

    static void handleStageException(const WorkflowPlan::Stage& stage, const mv::ManiVaultException& exception, SharedWorkflowExecutionContext stageContext);

    void compileWorkflow(
        const WorkflowPlan& workflowPlan,
        tf::Taskflow& taskflow,
        SharedWorkflowExecutionContext parentContext);

    void compileWorkflow(
        const WorkflowPlan& workflowPlan,
        tf::Subflow& subflow,
        SharedWorkflowExecutionContext parentContext);

    void addWorkflowFinishedNotification(const QString& workflowName, const SharedWorkflowResult& result, const QUuid& resultId);

    void executeCompiledJob(
        const WorkflowPlan::Job& job,
        tf::Subflow& subflow,
        SharedWorkflowExecutionContext jobContext);

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

        for (const auto& job : stage.getJobs()) {
            auto jobContext = stageContext->createChild(
                job.getName(),
                job.getWeight(),
                job.getProgressMode());

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

        for (const auto& job : stage.getJobs()) {
            auto jobContext = stageContext->createChild(
                job.getName(),
                job.getWeight(),
                job.getProgressMode());

            auto task = flow.emplace([this, job, jobContext](tf::Subflow& subflow) mutable {
                executeCompiledJob(job, subflow, jobContext);
                });

            result.starts.push_back(task);
            result.ends.push_back(task);
        }

        return result;
    }

    template<typename Flow>
    CompiledTasks compileStage(const WorkflowPlan::Stage& stage, Flow& flow, SharedWorkflowExecutionContext parentContext)
    {
        auto stageContext = parentContext->createChild(
            stage.getName(),
            stage.getWeight(),
            WorkflowPlan::JobProgressMode::Nested);

        if (stage.getConcurrencyMode() == WorkflowPlan::ConcurrencyMode::Sequential)
            return compileSequentialStage(stage, flow, stageContext);

        return compileParallelStage(stage, flow, stageContext);
    }

private:
    tf::Executor    _executor;  
};

