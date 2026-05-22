// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "WorkflowPlanExecutor.h"

#include <util/WorkflowResultRegistry.h>
#include <util/WorkflowMetric.h>
#include <util/WorkflowConsoleTraceSink.h>

#include <Task.h>

#include <QtConcurrent>
#include <QElapsedTimer>

#ifdef _DEBUG
	//#define WORKFLOW_PLAN_EXECUTOR_VERBOSE
#endif

//#define WORKFLOW_PLAN_EXECUTOR_VERBOSE

//#define USE_WORKFLOW_CONSOLE_TRACE_SINK

using namespace mv;
using namespace mv::util;

WorkflowPlanExecutor::WorkflowPlanExecutor(QObject* parent) :
	AbstractWorkflowPlanExecutor(parent)
{
}

SharedWorkflowResult WorkflowPlanExecutor::executeBlocking(UniqueWorkflowPlan workflowPlan, SharedWorkflowExecutionContext parentContext /*= nullptr*/, OptionalWorkflowExecutionOptions executionOptions /*= std::nullopt*/)
{
#ifdef WORKFLOW_PLAN_EXECUTOR_VERBOSE
    qDebug() << "Executing workflow plan:" << workflowPlan.getName() << "with" << workflowPlan.getStages().size() << "stage(s)";
#endif

    /*if (QThread::currentThread() == qApp->thread()) {
        throw ManiVaultException(
            SeverityLevel::Fatal,
            "executeBlocking() may not be called on the GUI thread",
            "workflow.blocking_on_gui_thread",
            "WorkflowPlanExecutor"
        );
    }*/

    static thread_local int blockingDepth = 0;

    if (blockingDepth > 0) {
        throw ManiVaultException(
            SeverityLevel::Fatal,
            "Nested executeBlocking() is not allowed",
            "workflow.nested_blocking_execution",
            "WorkflowPlanExecutor"
        );
    }

    blockingDepth++;

    auto guard = qScopeGuard([&] {
        blockingDepth--;
        });

    auto future = executeAsync(std::move(workflowPlan), parentContext, executionOptions);

    future.waitForFinished();

    return future.result();
}

WorkflowResultFuture WorkflowPlanExecutor::executeAsync(UniqueWorkflowPlan workflowPlan, SharedWorkflowExecutionContext parentContext /*= nullptr*/, OptionalWorkflowExecutionOptions executionOptions /*= std::nullopt*/)
{
    if (parentContext != nullptr) {
        const auto pendingWorkLabel = QString("Async workflow: %1").arg(workflowPlan->getName());
        const auto resolvedOptions  = executionOptions.value_or(parentContext->getState()->getExecutionOptions());

        auto childContext   = parentContext->createChild(workflowPlan->getName(), workflowPlan->getWeight(), WorkflowPlan::JobProgressMode::Automatic);
        auto future         = executeAsyncImpl(std::move(workflowPlan), resolvedOptions._reportProgress ? Task::GuiScope::Background : Task::GuiScope::None, resolvedOptions, childContext);

        parentContext->addPendingAsyncWork(future, pendingWorkLabel);
        
        return future;
    }

    const auto resolvedOptions = executionOptions.value_or({});

    return executeAsyncImpl(
        std::move(workflowPlan),
        resolvedOptions._reportProgress ? Task::GuiScope::Background : Task::GuiScope::None,
        resolvedOptions,
        nullptr
    );
}

QThreadPool& WorkflowPlanExecutor::getThreadPool(const SharedWorkflowExecutionContext& context)
{
    if (!context || !context->isValid())
        throw std::runtime_error("Invalid workflow execution context");

    return context->getThreadPool();
}

const QThreadPool& WorkflowPlanExecutor::getThreadPool(const SharedWorkflowExecutionContext& context) const
{
    if (!context || !context->isValid())
        throw std::runtime_error("Invalid workflow execution context");

    return context->getThreadPool();
}

WorkflowResultFuture WorkflowPlanExecutor::executeAsyncImpl(UniqueWorkflowPlan workflowPlan, Task::GuiScope guiScope, const WorkflowExecutionOptions& executionOptions, SharedWorkflowExecutionContext executionContext)
{
    auto state = std::make_shared<WorkflowResultFuture::State>();

    auto* watcher = new QFutureWatcher<SharedWorkflowResult>();

    state->watcher = watcher;

    connect(watcher, &QFutureWatcher<SharedWorkflowResult>::finished, watcher, &QObject::deleteLater, Qt::QueuedConnection);

    Task* task = nullptr;

    if (guiScope != Task::GuiScope::None) {
        task = new Task(nullptr, workflowPlan->getName());
        task->setGuiScopes({guiScope});
        task->setStatus(Task::Status::Running);
        task->setProgress(0.0f);
    }

    state->task = task;

    state->future = QtConcurrent::run([state, workflowPlan = std::move(workflowPlan), task, executionOptions, executionContext]() mutable -> SharedWorkflowResult {
        try {
            WorkflowPlanExecutor executor;

            if (executionContext != nullptr) {
                //WorkflowReporter::info("Nested workflow started");

                executor.executeImpl(*workflowPlan, executionContext);

                //WorkflowReporter::info("Nested workflow finished");

                return {};
            }

            return executor.executeRoot(*workflowPlan, task, executionOptions);
        }
        catch (...) {
            state->setException(std::current_exception());
            return {};
        }
    });

    watcher->setFuture(state->future);

    return WorkflowResultFuture(state);
}

SharedWorkflowResult WorkflowPlanExecutor::executeOnCurrentThread(WorkflowPlan& workflowPlan, Task* task, const WorkflowExecutionOptions& executionOptions /*= {}*/)
{
    return executeRoot(workflowPlan, task, executionOptions);
}

SharedWorkflowResult WorkflowPlanExecutor::executeOnCurrentThread(WorkflowPlan& workflowPlan, mv::Task* task, SharedWorkflowExecutionContext parentContext, OptionalWorkflowExecutionOptions executionOptions /*= std::nullopt*/)
{
    Q_UNUSED(task)

    return executeRoot(workflowPlan, task, executionOptions.value_or(WorkflowExecutionOptions{}));
}

SharedWorkflowResult WorkflowPlanExecutor::executeRoot(WorkflowPlan& workflowPlan, Task* task, const WorkflowExecutionOptions& executionOptions /*= {}*/)
{
    QElapsedTimer elapsedTimer;

    elapsedTimer.start();

    auto rootContext = WorkflowExecutionContext::makeRoot(workflowPlan.getName(), task, executionOptions);

    //WorkflowReporter::info("Workflow started", workflowPlan.getName());

#ifdef USE_WORKFLOW_CONSOLE_TRACE_SINK
    trace(WorkflowTraceEvent{
        ._type = WorkflowTraceEventType::WorkflowStarted,
        ._name = workflowPlan.getName(),
        ._contextId = rootContext->getId(),
        ._threadId = QThread::currentThreadId(),
        ._timestampNs = AbstractWorkflowTraceSink::currentTimestampNs()
    });
#endif

    const auto displayFailure = [workflowPlan, executionOptions](const QString& message) -> void {
        QMetaObject::invokeMethod(&help(), [workflowPlan, executionOptions, message]() {
            const auto title = QString("%1 failed").arg(workflowPlan.getName());

            if (executionOptions._addNotification) {
                help().addNotification(title, message);
            }

            qDebug() << QString("%1: %2").arg(title, message);
        });
    };

    try {
        executeImpl(workflowPlan, rootContext);
        
        //WorkflowReporter::info("Workflow finished", workflowPlan.getName());

#ifdef USE_WORKFLOW_CONSOLE_TRACE_SINK
        trace(WorkflowTraceEvent{
	        ._type = WorkflowTraceEventType::WorkflowFinished,
	        ._name = workflowPlan.getName(),
	        ._contextId = rootContext->getId(),
	        ._threadId = QThread::currentThreadId(),
	        ._timestampNs = AbstractWorkflowTraceSink::currentTimestampNs()
        });
#endif
    }
    catch (const ManiVaultException& exception) {
        rootContext->error(exception._message, exception._where, exception._details);

        displayFailure(exception._message);

        if (exception._severity == SeverityLevel::Fatal)
            throw;
    }
    catch (const std::exception& exception) {
        rootContext->error(exception.what(), workflowPlan.getName());

        displayFailure(QString::fromUtf8(exception.what()));

        throw;
    }
    catch (...) {
        rootContext->error("Workflow failed with unknown error", workflowPlan.getName());
        
        displayFailure("Unknown error");

        throw;
    }

    auto result = std::make_shared<WorkflowResult>(workflowPlan.getName());

    if (auto state = rootContext->getState()) {
	    result->setMetrics(state->metrics().snapshot());
    	result->setMessages(rootContext->getState()->collectMessages());
        result->setDuration(static_cast<std::uint64_t>(elapsedTimer.elapsed()));
    }

    const auto resultId = WorkflowResultRegistry::instance().add(result);

    if (executionOptions._addNotification) {
	    const auto url      = QString("app://workflow/results?workflowResultId=%1").arg(resultId.toString(QUuid::WithoutBraces));
        const auto title    = QString("%1 finished in %2").arg(workflowPlan.getName()).arg(getElapsedTimeHumanReadable(result->getDuration(), true));

    	QMetaObject::invokeMethod(&help(), [result, url, title]() {
            QString message;

    		if (!result->hasWarnings() && !result->hasErrors()) {
                message = QString("Completed successfully, see the <a href='%1'>report</a> for details").arg(QString("%1&levels=info").arg(url));
            }

            if (result->hasWarnings() && !result->hasErrors()) {
                message = QString("Completed with <a href=\"%1\">warnings</a>. Review the report.").arg(QString("%1&levels=warning").arg(url));
            }

            if (!result->hasWarnings() && result->hasErrors()) {
                message = QString("Completed with <a href=\"%1\">errors</a>. Review the report.").arg(QString("%1&levels=error").arg(url));
            }

            if (result->hasWarnings() && result->hasErrors()) {
                message = QString("Completed with <a href=\"%1\">warnings </a> and <a href=\"%1\">errors </a>. Review the report.").arg(QString("%1&levels=warning,error,critical").arg(url));
            }

            //message += WorkflowMetric::getWorkflowMetricsHtmlNotificationSummary(result->getMetrics());

            if (!message.isEmpty()) {
	            help().addNotification(title, message);
                qDebug() << title;
            }

    	}, Qt::QueuedConnection);
    }

    result->setValue(rootContext->takeResult());

    return result;
}

SharedWorkflowResult WorkflowPlanExecutor::executeChild(WorkflowPlan& workflowPlan, SharedWorkflowExecutionContext parentContext)
{
    auto childContext = parentContext->createChild(
        workflowPlan.getName(),
        workflowPlan.getWeight(),
        WorkflowPlan::JobProgressMode::Automatic);

    //WorkflowReporter::info("Nested workflow started", workflowPlan.getName());

#ifdef USE_WORKFLOW_CONSOLE_TRACE_SINK
    trace({
	    ._type = WorkflowTraceEventType::WorkflowStarted,
	    ._name = workflowPlan.getName(),
	    ._contextId = childContext->getId(),
	    ._parentContextId = childContext->getParentId(),
	    ._metadata = {
	        { "nested", true }
	    }
    });
#endif

    executeImpl(workflowPlan, childContext);

    //WorkflowReporter::info("Nested workflow finished", workflowPlan.getName());
    
#ifdef USE_WORKFLOW_CONSOLE_TRACE_SINK
    trace({
	    ._type = WorkflowTraceEventType::WorkflowFinished,
	    ._name = workflowPlan.getName(),
	    ._contextId = childContext->getId(),
	    ._parentContextId = childContext->getParentId(),
	    ._metadata = {
	        { "nested", true }
	    }
    });
#endif

    return {};
}

void WorkflowPlanExecutor::executeImpl(WorkflowPlan& workflowPlan, SharedWorkflowExecutionContext executionContext)
{
    executionContext = requireContext(executionContext, __FUNCTION__);

    std::exception_ptr primaryException;
    bool mainSucceeded = false;

    try {
        executeStageGroup(workflowPlan.getStages(), executionContext);
        mainSucceeded = true;
    }
    catch (...) {
        primaryException = std::current_exception();
    }

    try {
        if (mainSucceeded) {
            executeStageGroup(workflowPlan.getOnSuccessStages(), executionContext);
        }
        else {
            executeStageGroup(workflowPlan.getOnFailureStages(), executionContext);
        }
    }
    catch (...) {
        if (!primaryException)
            primaryException = std::current_exception();
    }

    try {
        executeStageGroup(workflowPlan.getFinallyStages(), executionContext);
    }
    catch (...) {
        if (!primaryException)
            primaryException = std::current_exception();
    }

    if (primaryException)
        std::rethrow_exception(primaryException);
}

void WorkflowPlanExecutor::executeStageGroup(const WorkflowPlan::Stages& stages, SharedWorkflowExecutionContext executionContext)
{
    executionContext = requireContext(executionContext, __FUNCTION__);

    const auto stageCount = stages.size();

    if (stageCount == 0)
        return;

    if (!executionContext)
        throw std::runtime_error("No active workflow execution context");

    QVector<SharedWorkflowExecutionContext> stageContexts;

    stageContexts.reserve(static_cast<int>(stageCount));

    for (int i = 0; i < stageCount; ++i) {
        const auto& stage = stages[i];
        stageContexts.push_back(executionContext->createChild(stage.getName(), stage.getWeight()));
    }

    for (int i = 0; i < stageCount; ++i) {

        const auto& stage = stages[i];

    	auto& stageContext = stageContexts[i];

        try {
            executeStage(stage, stageContext);
        }
        catch (const ManiVaultException& exception) {

            //WorkflowReporter::error("Stage finished", stage.getName());
            //ctx.setError(QString::fromStdString(e.what())); // or your reporting system

            if (exception._severity == SeverityLevel::Error || exception._severity == SeverityLevel::Fatal) {
                throw; // abort entire workflow
            }

            // NON-FATAL  continue to next stage
        }
        catch (const std::exception& exception) {
            Q_UNUSED(exception)
            //ctx.setError(QString::fromStdString(e.what()));

            // Treat unknown exceptions as fatal (recommended)
            throw;
        }
    }
}

void WorkflowPlanExecutor::executeStage(const WorkflowPlan::Stage& stage, SharedWorkflowExecutionContext stageContext)
{
    stageContext = requireContext(stageContext, __FUNCTION__);

    if (stage.getConcurrencyMode() == WorkflowPlan::ConcurrencyMode::Parallel && stage.containsGuiThreadJobs()) {
        throw ManiVaultException(SeverityLevel::Error, QString("Parallel stage '%1' contains GUI-thread jobs, which is not allowed.").arg(stage.getName()), "workflow.stage.parallel_gui_jobs", stage.getName());
    }

    stageContext->info("Stage started");

    auto state = stageContext->getState();

#ifdef USE_WORKFLOW_CONSOLE_TRACE_SINK
    trace({
	    ._type = WorkflowTraceEventType::StageStarted,
	    ._name = stage.getName(),
	    ._contextId = stageContext->getId(),
	    ._parentContextId = stageContext->getParentId(),
	    ._metadata = {
            { "jobCount", stage.getJobs().size() },
		    { "concurrencyMode", static_cast<int>(stage.getConcurrencyMode()) },
		    { "parallelEnabled", state ? state->getExecutionOptions()._parallel : false },
		    { "containsGuiThreadJobs", stage.containsGuiThreadJobs() }
	    }
    });
#endif

    try {
        WorkflowPlan::ConcurrencyMode effectiveMode = stage.getConcurrencyMode();

        if (state && !state->getExecutionOptions()._parallel) {
            stageContext->warning("Parallel execution is disabled, switching to sequential mode");

            effectiveMode = WorkflowPlan::ConcurrencyMode::Sequential;
        }

        switch (effectiveMode) {
	        case WorkflowPlan::ConcurrencyMode::Sequential:
	            executeSequentialJobs(stage, stageContext);
	            break;

	        case WorkflowPlan::ConcurrencyMode::Parallel:
	            executeParallelJobs(stage, stageContext);
	            break;
        }

        stageContext->info("Stage finished");

#ifdef USE_WORKFLOW_CONSOLE_TRACE_SINK
        trace({
		    ._type = WorkflowTraceEventType::StageFinished,
		    ._name = stage.getName(),
		    ._contextId = stageContext->getId(),
		    ._parentContextId = stageContext->getParentId()
        });
#endif

    }
    catch (const ManiVaultException& exception) {
        handleStageException(stage, exception, stageContext);

#ifdef USE_WORKFLOW_CONSOLE_TRACE_SINK
        trace({
            ._type = WorkflowTraceEventType::StageFinished,
            ._name = stage.getName(),
            ._contextId = stageContext->getId(),
            ._parentContextId = stageContext->getParentId()
        });
#endif
    }
    catch (const std::exception& exception) {
        stageContext->error(QString("Stage failed: %1").arg(exception.what()));
        
#ifdef USE_WORKFLOW_CONSOLE_TRACE_SINK
        trace({
            ._type = WorkflowTraceEventType::StageFinished,
            ._name = stage.getName(),
            ._contextId = stageContext->getId(),
            ._parentContextId = stageContext->getParentId()
        });
#endif

        throw;
    }
    catch (...) {
        stageContext->error("Stage failed with unknown error");

#ifdef USE_WORKFLOW_CONSOLE_TRACE_SINK
        trace({
            ._type = WorkflowTraceEventType::StageFinished,
            ._name = stage.getName(),
            ._contextId = stageContext->getId(),
            ._parentContextId = stageContext->getParentId()
        });
#endif

        throw;
    }
}

void WorkflowPlanExecutor::executeSequentialJobs(const WorkflowPlan::Stage& stage, SharedWorkflowExecutionContext stageContext)
{
#ifdef WORKFLOW_PLAN_EXECUTOR_VERBOSE
    qDebug() << "Executing sequential jobs for stage:" << stage.getName() << "in thread" << QThread::currentThread();
#endif

    stageContext = requireContext(stageContext, __FUNCTION__);

    const auto& jobs = stage.getJobs();
    const auto jobCount = jobs.size();

    if (jobCount == 0)
        return;

    QVector<SharedWorkflowExecutionContext> jobContexts;

    jobContexts.reserve(jobCount);

    for (int jobIndex = 0; jobIndex < jobCount; ++jobIndex) {
        const auto& job = jobs[jobIndex];
        jobContexts.push_back(stageContext->createChild(job.getName(), job.getWeight(), job.getProgressMode()));
    }

    for (int jobIndex = 0; jobIndex < jobCount; ++jobIndex) {
        auto& job = const_cast<WorkflowPlan::Job&>(jobs[jobIndex]);
        auto& jobContext = jobContexts[jobIndex];

        executeJob(job, jobContext);
    }

    for (auto& jobContext : jobContexts) {
        const auto pendingCount = jobContext->getPendingAsyncWorkCount();

        if (pendingCount == 0)
            continue;

#ifdef USE_WORKFLOW_CONSOLE_TRACE_SINK
        trace({
            ._type = WorkflowTraceEventType::PendingAsyncWorkWaitStarted,
            ._name = jobContext->getName(),
            ._contextId = jobContext->getId(),
            ._parentContextId = jobContext->getParentId(),
            ._metadata = {
                { "pendingAsyncWorkCount", pendingCount }
            }
        });
#endif

        jobContext->waitForPendingAsyncWork();

#ifdef USE_WORKFLOW_CONSOLE_TRACE_SINK
        trace({
            ._type = WorkflowTraceEventType::PendingAsyncWorkWaitFinished,
            ._name = jobContext->getName(),
            ._contextId = jobContext->getId(),
            ._parentContextId = jobContext->getParentId()
        });
#endif
    }
}

void WorkflowPlanExecutor::executeParallelJobs(const WorkflowPlan::Stage& stage, SharedWorkflowExecutionContext stageContext)
{
#ifdef WORKFLOW_PLAN_EXECUTOR_VERBOSE
    qDebug() << "Executing parallel jobs for stage:"
        << stage.getName()
        << "in thread"
        << QThread::currentThread();
#endif

    stageContext = requireContext(stageContext, __FUNCTION__);

    const auto& jobs = stage.getJobs();
    const auto jobCount = jobs.size();

    if (jobCount == 0)
        return;

    for (const auto& job : jobs) {
        if (job.getThreadAffinity() == WorkflowPlan::JobThreadAffinity::GuiThread) {
            throw ManiVaultException(
                SeverityLevel::Error,
                QString("GUI-thread job '%1' is not allowed in a parallel stage")
                .arg(job.getName()),
                "workflow.parallel.gui_thread_job",
                stage.getName());
        }
    }

#ifdef USE_WORKFLOW_CONSOLE_TRACE_SINK
    trace({
        ._type = WorkflowTraceEventType::ParallelStageStarted,
        ._name = stage.getName(),
        ._contextId = stageContext->getId(),
        ._parentContextId = stageContext->getParentId(),
        ._metadata = {
            { "jobCount", jobCount },
            { "maxThreadCount", getThreadPool(stageContext).maxThreadCount() },
            { "activeThreadCount", getThreadPool(stageContext).activeThreadCount() }
        }
    });
#endif

    QVector<SharedWorkflowExecutionContext> jobContexts;

    jobContexts.reserve(jobCount);

    for (int jobIndex = 0; jobIndex < jobCount; ++jobIndex) {
        const auto& job = jobs[jobIndex];

        jobContexts.push_back(
            stageContext->createChild(
                job.getName(),
                job.getWeight(),
                job.getProgressMode()));
    }

    QFutureSynchronizer<void> synchronizer;
    std::mutex exceptionMutex;
    std::exception_ptr firstException = nullptr;

    for (int jobIndex = 0; jobIndex < jobCount; ++jobIndex) {
        auto jobContext = jobContexts[jobIndex];
        auto job        = jobs[jobIndex];

        const auto stageNameCopy = stage.getName();

#ifdef USE_WORKFLOW_CONSOLE_TRACE_SINK
        trace({
            ._type = WorkflowTraceEventType::ParallelJobSubmitted,
            ._name = job.getName(),
            ._contextId = jobContext->getId(),
            ._parentContextId = jobContext->getParentId(),
            ._metadata = {
                { "stage", stage.getName() },
                { "jobIndex", jobIndex },
                { "jobCount", jobCount },
                { "maxThreadCount", getThreadPool(stageContext).maxThreadCount() },
                { "activeThreadCount", getThreadPool(stageContext).activeThreadCount() }
            }
        });
#endif

        synchronizer.addFuture(QtConcurrent::run(
            &getThreadPool(jobContext),
            [this,
            job,
            jobContext,
            stageName = stage.getName(),
            jobIndex = jobIndex,
            jobCount,
            &exceptionMutex,
            &firstException]() mutable {
#ifdef USE_WORKFLOW_CONSOLE_TRACE_SINK
                trace({
                    ._type = WorkflowTraceEventType::ParallelJobStarted,
                    ._name = job.getName(),
                    ._contextId = jobContext->getId(),
                    ._parentContextId = jobContext->getParentId(),
                    ._metadata = {
                        { "stage", stageName },
                        { "jobIndex", jobIndex },
                        { "jobCount", jobCount },
                        { "maxThreadCount", getThreadPool(stageContext).maxThreadCount() },
                        { "activeThreadCount", getThreadPool(stageContext).activeThreadCount() }
                    }
                });
#endif

                try {
                    executeJob(job, jobContext);

#ifdef USE_WORKFLOW_CONSOLE_TRACE_SINK
                    trace({
                        ._type = WorkflowTraceEventType::ParallelJobFinished,
                        ._name = job.getName(),
                        ._contextId = jobContext->getId(),
                        ._parentContextId = jobContext->getParentId(),
                        ._metadata = {
                            { "stage", stageName },
                            { "jobIndex", jobIndex }
                        }
                    });
#endif
                }
                catch (...) {
                    {
                        std::lock_guard<std::mutex> lock(exceptionMutex);

                        if (!firstException)
                            firstException = std::current_exception();
                    }

#ifdef USE_WORKFLOW_CONSOLE_TRACE_SINK
                    trace({
                        ._type = WorkflowTraceEventType::ParallelJobFailed,
                        ._name = job.getName(),
                        ._contextId = jobContext->getId(),
                        ._parentContextId = jobContext->getParentId(),
                        ._metadata = {
                            { "stage", stageName },
                            { "jobIndex", jobIndex }
                        }
                    });
#endif
                }
            }
        ));
    }

    synchronizer.waitForFinished();

    for (auto& jobContext : jobContexts) {
        if (jobContext->getPendingAsyncWorkCount() == 0)
            continue;

        jobContext->waitForPendingAsyncWork();
    }

    if (firstException) {
#ifdef USE_WORKFLOW_CONSOLE_TRACE_SINK
        trace({
            ._type = WorkflowTraceEventType::ParallelStageFailed,
            ._name = stage.getName(),
            ._contextId = stageContext->getId(),
            ._parentContextId = stageContext->getParentId()
        });
#endif

        std::rethrow_exception(firstException);
    }

#ifdef USE_WORKFLOW_CONSOLE_TRACE_SINK
    trace({
        ._type = WorkflowTraceEventType::ParallelStageFinished,
        ._name = stage.getName(),
        ._contextId = stageContext->getId(),
        ._parentContextId = stageContext->getParentId(),
        ._metadata = {
            { "jobCount", jobCount },
            { "maxThreadCount", getThreadPool(stageContext).maxThreadCount() },
            { "activeThreadCount", getThreadPool(stageContext).activeThreadCount() }
        }
    });
#endif
}

void WorkflowPlanExecutor::executeJobOnGuiThread(WorkflowPlan::Job& job, SharedWorkflowExecutionContext jobContext)
{
    jobContext = requireContext(jobContext, __FUNCTION__);

    auto& dispatcher = Application::workflowGuiThreadDispatcher();

    auto exceptionPtr = std::make_shared<std::exception_ptr>();

    auto runOnGuiThread = [job = std::move(job),
        jobContext,
        exceptionPtr]() mutable {
        try {
            job.run(jobContext);
        }
        catch (...) {
            *exceptionPtr = std::current_exception();
        }
    };

    if (QThread::currentThread() == dispatcher.thread()) {
        runOnGuiThread();
    }
    else {
        QMetaObject::invokeMethod(
            &dispatcher,
            std::move(runOnGuiThread),
            Qt::BlockingQueuedConnection
        );
    }

    if (*exceptionPtr)
        std::rethrow_exception(*exceptionPtr);
}

void WorkflowPlanExecutor::executeJobOnWorkerThread(mv::util::WorkflowPlan::Job& job, SharedWorkflowExecutionContext jobContext)
{
#ifdef WORKFLOW_PLAN_EXECUTOR_VERBOSE
    qDebug() << "Executing job on worker thread:" << job.getName() << "in thread" << QThread::currentThread();
#endif

    jobContext = requireContext(jobContext, __FUNCTION__);

    job.run(jobContext);
}

void WorkflowPlanExecutor::executeJob(WorkflowPlan::Job& job, SharedWorkflowExecutionContext jobContext)
{
    jobContext->info("Job started");

#ifdef USE_WORKFLOW_CONSOLE_TRACE_SINK
    trace({
	    ._type = WorkflowTraceEventType::JobStarted,
	    ._name = job.getName(),
	    ._contextId = jobContext->getId(),
	    ._parentContextId = jobContext->getParentId()
    });
#endif

    jobContext = requireContext(jobContext, __FUNCTION__);

    try {
        switch (job.getThreadAffinity()) {
	        case WorkflowPlan::JobThreadAffinity::CurrentWorkerThread:
	            executeJobOnWorkerThread(job, jobContext);
	            break;

	        case WorkflowPlan::JobThreadAffinity::GuiThread:
	            executeJobOnGuiThread(job, jobContext);
	            break;
        }

        if (job.getProgressMode() == WorkflowPlan::JobProgressMode::Atomic) {
            jobContext->setProgress(1.0);
        }
        else if (job.getProgressMode() == WorkflowPlan::JobProgressMode::Automatic && !jobContext->hasProgressChildren()) {
            jobContext->setProgress(1.0);
        }

        jobContext->info("Job finished");

#ifdef USE_WORKFLOW_CONSOLE_TRACE_SINK
        trace({
		    ._type = WorkflowTraceEventType::JobFinished,
		    ._name = job.getName(),
		    ._contextId = jobContext->getId(),
		    ._parentContextId = jobContext->getParentId()
        });
#endif
    }
    catch (...) {
        if (job.getProgressMode() == WorkflowPlan::JobProgressMode::Atomic || (job.getProgressMode() == WorkflowPlan::JobProgressMode::Automatic && !jobContext->hasProgressChildren())) {
            jobContext->setProgress(1.0);
        }

#ifdef USE_WORKFLOW_CONSOLE_TRACE_SINK
        trace({
		    ._type = WorkflowTraceEventType::JobFinished,
		    ._name = job.getName(),
		    ._contextId = jobContext->getId(),
		    ._parentContextId = jobContext->getParentId()
        });
#endif
        throw;
    }
}

void WorkflowPlanExecutor::handleStageException(const WorkflowPlan::Stage& stage, const ManiVaultException& exception, SharedWorkflowExecutionContext stageContext)
{
    stageContext->message(exception._severity, exception._message, stage.getName(), exception._details);

    if (exception._severity == SeverityLevel::Error || exception._severity == SeverityLevel::Fatal)
        throw;
}

