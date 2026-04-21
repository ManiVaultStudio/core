// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "WorkflowPlanExecutor.h"

#include <Task.h>

#ifdef _DEBUG
	#define WORKFLOW_PLAN_EXECUTOR_VERBOSE
#endif

using namespace mv;
using namespace mv::util;

WorkflowResult WorkflowPlanExecutor::execute(WorkflowPlan& workflowPlan, bool showProgress)
{
    WorkflowResult result;

    beginTimer();
	{
		if (auto* currentContext = WorkflowExecutionContext::current())
			result = executeChild(workflowPlan, *currentContext);
		else
			result = executeRoot(workflowPlan, Task::GuiScope::Modal);
	}
    endTimer(result);

    return result;
}

WorkflowResult WorkflowPlanExecutor::executeAsync(mv::util::WorkflowPlan& workflowPlan, bool showProgress)
{
    WorkflowResult result;

    //beginTimer();
    //{
    //    if (auto* currentContext = WorkflowExecutionContext::current())
    //        result = executeChild(workflowPlan, *currentContext);
    //    else
    //        result = executeRoot(workflowPlan, taskGuiScope);
    //}
    //endTimer(result);

    return result;
}

WorkflowResult WorkflowPlanExecutor::executeRoot(const WorkflowPlan& workflowPlan, Task::GuiScope taskGuiScope)
{
    auto rootContext = WorkflowExecutionContext::makeRoot(workflowPlan.getName(), taskGuiScope);
    WorkflowExecutionScope rootScope(rootContext);

    WorkflowReporter::info("Workflow started", workflowPlan.getName());

    try {
        executeImpl(workflowPlan);
        //rootContext.setProgress(1.0);
        WorkflowReporter::info("Workflow finished", workflowPlan.getName());
    }
    catch (const std::exception& e) {
        WorkflowReporter::error(QString("Workflow failed: %1").arg(QString::fromUtf8(e.what())),
            workflowPlan.getName());
    }
    catch (...) {
        WorkflowReporter::error("Workflow failed with unknown error",
            workflowPlan.getName());
    }

    return WorkflowResult(rootContext);
}

WorkflowResult WorkflowPlanExecutor::executeChild(const WorkflowPlan& workflowPlan, WorkflowExecutionContext& parentContext)
{
    auto childContext = parentContext.createChild(workflowPlan.getName(), 1.0);
    WorkflowExecutionScope childScope(childContext);

    WorkflowReporter::info("Nested workflow started", workflowPlan.getName());

    try {
        executeImpl(workflowPlan);
        //childContext.setProgress(1.0);
        WorkflowReporter::info("Nested workflow finished", workflowPlan.getName());
    }
    catch (const std::exception& e) {
        WorkflowReporter::error(QString("Nested workflow failed: %1").arg(QString::fromUtf8(e.what())),
            workflowPlan.getName());
    }
    catch (...) {
        WorkflowReporter::error("Nested workflow failed with unknown error",
            workflowPlan.getName());
    }

    return WorkflowResult(childContext);
}

void WorkflowPlanExecutor::executeImpl(const WorkflowPlan& workflowPlan)
{
    const auto& stages = workflowPlan.getStages();
    const auto stageCount = stages.size();

    if (stageCount == 0)
        return;

    auto* currentContext = WorkflowExecutionContext::current();
    if (currentContext == nullptr)
        throw std::runtime_error("No active workflow execution context");

    QVector<WorkflowExecutionContext> stageContexts;
    stageContexts.reserve(stageCount);

    for (int stageIndex = 0; stageIndex < stageCount; ++stageIndex) {
        const auto& stage = stages[stageIndex];
        stageContexts.push_back(currentContext->createChild(stage.getName(), stage.getWeight()));
    }

    for (int stageIndex = 0; stageIndex < stageCount; ++stageIndex) {
        const auto& stage = stages[stageIndex];
        auto& stageContext = stageContexts[stageIndex];

        executeStage(stage, stageContext);
    }
}

void WorkflowPlanExecutor::executeStage(const WorkflowPlan::Stage& stage, WorkflowExecutionContext& stageContext)
{
    WorkflowExecutionScope stageScope(stageContext);

    WorkflowReporter::info("Stage started", stage.getName());

    try {
        switch (stage.getConcurrencyMode()) {
        case WorkflowPlan::ConcurrencyMode::Sequential:
            executeSequentialJobs(stage, stageContext);
            break;

        case WorkflowPlan::ConcurrencyMode::Parallel:
            executeParallelJobs(stage, stageContext);
            break;
        }

        WorkflowReporter::info("Stage finished", stage.getName());
    }
    catch (const std::exception& e) {
        WorkflowReporter::error(QString("Stage failed: %1").arg(e.what()), stage.getName());
        throw;
    }
    catch (...) {
        WorkflowReporter::error("Stage failed with unknown error", stage.getName());
        throw;
    }
}

void WorkflowPlanExecutor::executeSequentialJobs(const mv::util::WorkflowPlan::Stage& stage, mv::util::WorkflowExecutionContext& stageContext)
{
    const auto& jobs = stage.getJobs();

    const auto jobCount = jobs.size();

    if (jobCount == 0) {
        //stageContext.setProgress(1.0);
        return;
    }

    QVector<WorkflowExecutionContext> jobContexts;
    jobContexts.reserve(jobCount);

    for (int jobIndex = 0; jobIndex < jobCount; ++jobIndex) {
        const auto& job = jobs[jobIndex];
        jobContexts.push_back(stageContext.createChild(job.getName(), 1.0));
    }

    for (int jobIndex = 0; jobIndex < jobCount; ++jobIndex) {
        const auto& job = jobs[jobIndex];
        auto& jobContext = jobContexts[jobIndex];

        WorkflowExecutionScope jobScope(jobContext);

        WorkflowReporter::info("Job started", job.getName());

        try {
            const_cast<WorkflowPlan::Job&>(job).run();

            if (!jobContext.hasProgressChildren())
                jobContext.setProgress(1.0);
        }
        catch (const std::exception& e) {
            Q_UNUSED(e)

            if (!jobContext.hasProgressChildren())
                jobContext.setProgress(1.0);
            throw;
        }
        catch (...) {
            if (!jobContext.hasProgressChildren())
                jobContext.setProgress(1.0);
            throw;
        }
    }
}

void WorkflowPlanExecutor::executeParallelJobs(const mv::util::WorkflowPlan::Stage& stage, mv::util::WorkflowExecutionContext& stageContext)
{
    const auto& jobs = stage.getJobs();

    if (jobs.empty()) {
        //stageContext.setProgress(1.0);
        return;
    }

    QFutureSynchronizer<void> synchronizer;

    std::mutex exceptionMutex;
    std::exception_ptr firstException = nullptr;
    std::atomic_bool failureSeen = false;

    QVector<WorkflowExecutionContext> jobContexts;
    jobContexts.reserve(jobs.size());

    for (int jobIndex = 0; jobIndex < jobs.size(); ++jobIndex) {
        const auto& job = jobs[jobIndex];
        jobContexts.push_back(stageContext.createChild(job.getName(), 1.0));
    }

    for (int jobIndex = 0; jobIndex < jobs.size(); ++jobIndex) {
        const auto& job = jobs[jobIndex];

        auto jobContext = jobContexts[jobIndex];

        //auto jobContext = stageContext.createChild(job.getName(), 1.0);

        synchronizer.addFuture(QtConcurrent::run(
            [jobContext, &job, &exceptionMutex, &firstException, &failureSeen]() mutable {
                WorkflowExecutionScope jobScope(jobContext);

                WorkflowReporter::info("Job started", job.getName());

                try {
                    const_cast<WorkflowPlan::Job&>(job).run();
                    if (!jobContext.hasProgressChildren())
                        jobContext.setProgress(1.0);
                    WorkflowReporter::info("Job finished", job.getName());
                }
                catch (...) {
                    jobContext.setProgress(1.0);

                    {
                        std::lock_guard<std::mutex> lock(exceptionMutex);
                        if (!firstException)
                            firstException = std::current_exception();
                    }

                    failureSeen = true;

                    try {
                        throw;
                    }
                    catch (const std::exception& e) {
                        WorkflowReporter::error(QString("Job failed: %1").arg(e.what()),
                            job.getName());
                    }
                    catch (...) {
                        WorkflowReporter::error("Job failed with unknown error",
                            job.getName());
                    }
                }
            }));
    }

    synchronizer.waitForFinished();

    if (firstException)
        std::rethrow_exception(firstException);
}
