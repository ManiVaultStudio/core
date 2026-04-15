// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "WorkflowPlanExecutor.h"

#include <util/OperationContext.h>
#include <util/OperationContextScope.h>

#ifdef _DEBUG
	#define WORKFLOW_PLAN_EXECUTOR_VERBOSE
#endif

using namespace mv::util;

WorkflowResult WorkflowPlanExecutor::execute(WorkflowPlan& workflowPlan)
{
    Workflow workflow(workflowPlan, OperationContextScope::getShared());

    workflow.beginRun();
    {
        for (const auto& stage : workflowPlan.getStages()) {
            runStage(stage);
        }
    }
    workflow.endRun();

    return workflow.getResult();
}

void WorkflowPlanExecutor::runStage(const mv::util::WorkflowPlan::Stage& stage)
{
    switch (stage.getConcurrencyMode()) {
	    case WorkflowPlan::ConcurrencyMode::Sequential:
	        runStageInSequence(stage);
	        break;

	    case WorkflowPlan::ConcurrencyMode::Parallel:
	        runStageInParallel(stage);
	        break;
    }
}

void WorkflowPlanExecutor::runStageInSequence(const mv::util::WorkflowPlan::Stage& stage)
{
    for (auto& job : stage.getJobs()) {
        job.run();
    }
}

void WorkflowPlanExecutor::runStageInParallel(const mv::util::WorkflowPlan::Stage& stage)
{
    QList<QFuture<void>> futures;
    futures.reserve(stage.getJobs().size());

    QThreadPool* pool = QThreadPool::globalInstance();

    std::mutex errorMutex;
    std::exception_ptr firstException = nullptr;

    for (auto job : stage.getJobs()) {  // copy
        futures.push_back(QtConcurrent::run(pool, [job, &errorMutex, &firstException]() mutable {
            try {
                job.run();
            }
            catch (...) {
                std::lock_guard<std::mutex> lock(errorMutex);
                if (!firstException)
                    firstException = std::current_exception();
            }
        }));
    }

    for (auto& future : futures) {
        future.waitForFinished();
    }

    if (firstException)
        std::rethrow_exception(firstException);
}
