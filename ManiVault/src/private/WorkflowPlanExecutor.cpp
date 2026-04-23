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

WorkflowPlanExecutor::WorkflowPlanExecutor()
{
    _threadPool.setObjectName("WorkflowPlanExecutorPool");

    // Reasonable default. You can tune this.
    _threadPool.setMaxThreadCount(QThread::idealThreadCount());

    // Optional: keep threads warm a bit longer
    _threadPool.setExpiryTimeout(30'000);
}

WorkflowResult WorkflowPlanExecutor::execute(WorkflowPlan& workflowPlan, bool showProgress, WorkflowExecutionOptions executionOptions /*= {}*/)
{
    WorkflowResult result;
    
    if (auto* currentContext = WorkflowExecutionContext::current()) {
        result = executeChild(workflowPlan, *currentContext);
    }
    else {
        beginTimer();
	    {
		    auto future = executeAsyncImpl(workflowPlan, showProgress ? Task::GuiScope::Modal : Task::GuiScope::None, std::move(executionOptions));

        	if (QThread::currentThread() == qApp->thread()) {
        		QEventLoop loop;

        		auto* watcher = future.getWatcher();

        		connect(watcher, &QFutureWatcher<WorkflowResult>::finished, &loop, &QEventLoop::quit);

        		loop.exec();
        	}
        	else {
        		future.waitForFinished();
        	}

        	result = future.result();
	    }
        endTimer(result);
    }

    return result;
}

WorkflowResultFuture WorkflowPlanExecutor::executeAsync(mv::util::WorkflowPlan& workflowPlan, bool showProgress, WorkflowExecutionOptions executionOptions /*= {}*/)
{
    if (WorkflowExecutionContext::current() != nullptr) {
        throw std::runtime_error("executeAsync() cannot be called from within an active workflow context");
    }

    WorkflowPlan workflowPlanCopy = workflowPlan;

    return executeAsyncImpl(std::move(workflowPlanCopy), showProgress ? Task::GuiScope::Background : Task::GuiScope::None);
}

QThreadPool& WorkflowPlanExecutor::threadPool()
{
    return _threadPool;
}

const QThreadPool& WorkflowPlanExecutor::threadPool() const
{
    return _threadPool;
}

WorkflowResultFuture WorkflowPlanExecutor::executeAsyncImpl(WorkflowPlan workflowPlan, Task::GuiScope guiScope, mv::util::WorkflowExecutionOptions executionOptions /*= {}*/)
{
    auto state = std::make_shared<WorkflowResultFuture::State>();

    Task* task = nullptr;

    if (guiScope != Task::GuiScope::None) {
        task = new Task(nullptr, workflowPlan.getName());
        task->setGuiScopes({guiScope});
        task->setStatus(Task::Status::Running);
        task->setProgress(0.0f);
    }

    state->task = task;

    state->future = QtConcurrent::run([workflowPlan = std::move(workflowPlan), task, executionOptions = std::move(executionOptions)]() mutable -> WorkflowResult {
    	WorkflowPlanExecutor executor;
        return executor.executeOnCurrentThread(workflowPlan, task, std::move(executionOptions));
	});

    auto* watcher = new QFutureWatcher<WorkflowResult>();
    watcher->setFuture(state->future);
    state->watcher = watcher;

    if (task != nullptr) {
        connect(watcher, &QFutureWatcher<WorkflowResult>::finished, task, [watcher, task]() {
            const auto result = watcher->result();

            task->setProgress(1.0f);
            task->setFinished();
        },
        Qt::QueuedConnection);
    }

    connect(watcher, &QFutureWatcher<WorkflowResult>::finished, watcher,&QObject::deleteLater, Qt::QueuedConnection);

    return WorkflowResultFuture(state);
}

WorkflowResult WorkflowPlanExecutor::executeOnCurrentThread(WorkflowPlan& workflowPlan, Task* task, WorkflowExecutionOptions executionOptions /*= {}*/)
{
    WorkflowResult result;

    if (auto* currentContext = WorkflowExecutionContext::current())
        result = executeChild(workflowPlan, *currentContext);
    else
        result = executeRoot(workflowPlan, task, std::move(executionOptions));

    return result;
}

WorkflowResult WorkflowPlanExecutor::executeRoot(const WorkflowPlan& workflowPlan, Task* task, WorkflowExecutionOptions executionOptions /*= {}*/)
{
    auto rootContext = WorkflowExecutionContext::makeRoot(workflowPlan.getName(), task, std::move(executionOptions));
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
    auto childContext = parentContext.createChild(workflowPlan.getName(), workflowPlan.getWeight());
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
    if (stage.getConcurrencyMode() == WorkflowPlan::ConcurrencyMode::Parallel && stage.containsGuiThreadJobs()) {
        throw std::runtime_error(QString("Parallel stage '%1' contains GUI-thread jobs, which is not allowed").arg(stage.getName()).toStdString());
    }

    WorkflowExecutionScope stageScope(stageContext);

    WorkflowReporter::info("Stage started", stage.getName());

    try {
        WorkflowPlan::ConcurrencyMode effectiveMode = stage.getConcurrencyMode();

        auto* currentContext = WorkflowExecutionContext::current();
        if (currentContext != nullptr) {
            const auto state = currentContext->getState();
            if (state && state->getExecutionOptions()._parallelizationOverride == ParallelizationOverride::ForceSequential) {
                effectiveMode = WorkflowPlan::ConcurrencyMode::Sequential;
            }
        }

        switch (effectiveMode) {
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

void WorkflowPlanExecutor::executeSequentialJobs(const WorkflowPlan::Stage& stage, WorkflowExecutionContext& stageContext)
{
    const auto& jobs = stage.getJobs();
    const auto jobCount = jobs.size();

    if (jobCount == 0)
        return;

    QVector<WorkflowExecutionContext> jobContexts;
    jobContexts.reserve(jobCount);

    for (int jobIndex = 0; jobIndex < jobCount; ++jobIndex) {
        const auto& job = jobs[jobIndex];
        jobContexts.push_back(stageContext.createChild(job.getName(), 1.0));
    }

    for (int jobIndex = 0; jobIndex < jobCount; ++jobIndex) {
        auto& job = const_cast<WorkflowPlan::Job&>(jobs[jobIndex]);
        auto& jobContext = jobContexts[jobIndex];

        executeJob(job, jobContext);
    }
}

void WorkflowPlanExecutor::executeParallelJobs(const WorkflowPlan::Stage& stage, WorkflowExecutionContext& stageContext)
{
    const auto& jobs = stage.getJobs();
    const int jobCount = jobs.size();

    if (jobCount == 0)
        return;

    for (const auto& job : jobs) {
        if (job.getThreadAffinity() == WorkflowPlan::JobThreadAffinity::GuiThread) {
            throw std::runtime_error(
                QString("GUI-thread job '%1' is not allowed in a parallel stage")
                .arg(job.getName())
                .toStdString());
        }
    }

    QVector<WorkflowExecutionContext> jobContexts;
    jobContexts.reserve(jobCount);

    for (int i = 0; i < jobCount; ++i) {
        const auto& job = jobs[i];
        jobContexts.push_back(stageContext.createChild(job.getName(), job.getWeight()));
    }

    QFutureSynchronizer<void> synchronizer;
    std::mutex exceptionMutex;
    std::exception_ptr firstException = nullptr;

    for (int i = 0; i < jobCount; ++i) {
        auto* job = &jobs[i];
        auto jobContext = jobContexts[i];

        synchronizer.addFuture(QtConcurrent::run(
            &threadPool(),
            [this, job, jobContext, &exceptionMutex, &firstException]() mutable {
                try {
                    executeJob(*job, jobContext);
                }
                catch (...) {
                    std::lock_guard<std::mutex> lock(exceptionMutex);
                    if (!firstException)
                        firstException = std::current_exception();
                }
            }));
    }

    synchronizer.waitForFinished();

    if (firstException)
        std::rethrow_exception(firstException);
}

void WorkflowPlanExecutor::executeJobOnGuiThread(const WorkflowPlan::Job& job, WorkflowExecutionContext& jobContext)
{
    auto& dispatcher = Application::workflowGuiThreadDispatcher();
    std::exception_ptr exceptionPtr;

    QMetaObject::invokeMethod(
        &dispatcher,
        [&job, &jobContext, &exceptionPtr]() {
            try {
                WorkflowExecutionScope scope(jobContext);
                job.run();

                if (!jobContext.hasProgressChildren())
                    jobContext.setProgress(1.0);
            }
            catch (...) {
                exceptionPtr = std::current_exception();
            }
        },
        Qt::BlockingQueuedConnection);

    if (exceptionPtr)
        std::rethrow_exception(exceptionPtr);
}

void WorkflowPlanExecutor::executeJobOnWorkerThread(const WorkflowPlan::Job& job, WorkflowExecutionContext& jobContext)
{
    WorkflowExecutionScope scope(jobContext);

    job.run();

    if (!jobContext.hasProgressChildren())
        jobContext.setProgress(1.0);
}

void WorkflowPlanExecutor::executeJob(const WorkflowPlan::Job& job, WorkflowExecutionContext& jobContext)
{
    WorkflowReporter::info("Job started", job.getName());

    try {
        switch (job.getThreadAffinity()) {
	        case WorkflowPlan::JobThreadAffinity::CurrentWorkerThread:
	            executeJobOnWorkerThread(job, jobContext);
	            break;

	        case WorkflowPlan::JobThreadAffinity::GuiThread:
	            executeJobOnGuiThread(job, jobContext);
	            break;
        }

        WorkflowReporter::info("Job finished", job.getName());
    }
    catch (const std::exception& e) {
        WorkflowReporter::error(QString("Job failed: %1").arg(e.what()), job.getName());
        throw;
    }
    catch (...) {
        WorkflowReporter::error("Job failed with unknown error", job.getName());
        throw;
    }
}

void WorkflowPlanExecutor::setMaxWorkerThreadCount(int count)
{
    _threadPool.setMaxThreadCount(std::max(1, count));
}

int WorkflowPlanExecutor::getMaxWorkerThreadCount() const
{
    return _threadPool.maxThreadCount();
}
