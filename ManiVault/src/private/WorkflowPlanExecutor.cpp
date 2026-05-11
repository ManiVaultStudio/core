// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "WorkflowPlanExecutor.h"

#include <util/WorkflowResultRegistry.h>
#include <util/WorkflowConsoleTraceSink.h>

#include <Task.h>

#include <QtConcurrent>
#include <QElapsedTimer>

#ifdef _DEBUG
	#define WORKFLOW_PLAN_EXECUTOR_VERBOSE
#endif

//#define WORKFLOW_PLAN_EXECUTOR_VERBOSE

#define USE_WORKFLOW_CONSOLE_TRACE_SINK

using namespace mv;
using namespace mv::util;

WorkflowPlanExecutor::WorkflowPlanExecutor(QObject* parent) :
	AbstractWorkflowPlanExecutor(parent)
{
}

SharedWorkflowResult WorkflowPlanExecutor::executeBlocking(WorkflowPlan& workflowPlan, WorkflowExecutionOptions executionOptions /*= {}*/)
{
#ifdef WORKFLOW_PLAN_EXECUTOR_VERBOSE
    qDebug() << "Executing workflow plan:" << workflowPlan.getName() << "with" << workflowPlan.getStages().size() << "stage(s)";
#endif

    SharedWorkflowResult result;

    try {
	    if (auto* currentContext = WorkflowExecutionContext::current()) {
	        result = executeChild(workflowPlan, *currentContext);
	    }
	    else {
		    auto future = executeAsyncImpl(workflowPlan, executionOptions._reportProgress ? Task::GuiScope::Modal : Task::GuiScope::None, executionOptions, nullptr);

	        if (QThread::currentThread() == qApp->thread()) {
        		QEventLoop loop;

        		auto* watcher = future.getWatcher();

                connect(watcher, &QFutureWatcher<SharedWorkflowResult>::finished, future.getTask(), [watcher, future]() {
                    if (future.getState()->hasException()) {
                        future.getTask()->setFinished();
                        return;
                    }

                    const auto result = watcher->result();

                    future.getTask()->setProgress(1.0f);
                    future.getTask()->setFinished();
                },
                Qt::QueuedConnection);

                connect(watcher, &QFutureWatcher<SharedWorkflowResult>::finished, &loop, &QEventLoop::quit);

        		loop.exec();
	        }
	        else {
        		future.waitForFinished();
	        }

	        result = future.result();

            future.getState()->rethrowExceptionIfAny();
	    }
    }
    catch (const ManiVaultException& exception) {
        WorkflowReporter::message(exception._severity, exception._message, workflowPlan.getName(), exception._code, exception._scope, exception._details);

        // displayFailure(exception._message);
        //throw;
    }
    catch (const std::exception& exception) {
        WorkflowReporter::error(QString("Workflow '%1' failed: %2").arg(workflowPlan.getName(), exception.what()),workflowPlan.getName());
        // displayFailure(exception.what());

        //throw;
    }
    catch (...) {
        WorkflowReporter::error(QString("Workflow '%1' failed with an unknown error.").arg(workflowPlan.getName()), workflowPlan.getName());

        // displayFailure("Unknown exception");

        //throw;
    }

    return result;
}

WorkflowResultFuture WorkflowPlanExecutor::executeAsync(WorkflowPlan& workflowPlan, WorkflowExecutionOptions executionOptions)
{
    const auto pendingWorkLabel = QString("Async workflow: %1").arg(workflowPlan.getName());

    auto* parentContext = WorkflowExecutionContext::current();

    auto future = executeAsyncImpl(WorkflowPlan(workflowPlan), executionOptions._reportProgress ? Task::GuiScope::Background : Task::GuiScope::None, executionOptions, parentContext);

    if (parentContext) {
        parentContext->addPendingAsyncWork(future, pendingWorkLabel);
    }

    return future;
}

QThreadPool& WorkflowPlanExecutor::getThreadPool()
{
    return WorkflowExecutionContext::current()->getThreadPool();
}

const QThreadPool& WorkflowPlanExecutor::getThreadPool() const
{
    return WorkflowExecutionContext::current()->getThreadPool();
}

WorkflowResultFuture WorkflowPlanExecutor::executeAsyncImpl(WorkflowPlan workflowPlan, Task::GuiScope guiScope, mv::util::WorkflowExecutionOptions executionOptions, WorkflowExecutionContext* parentContext)
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

    state->future = QtConcurrent::run([state, workflowPlan = std::move(workflowPlan), task, executionOptions, parentContext]() mutable -> SharedWorkflowResult {
        try {
            WorkflowPlanExecutor executor;

            if (parentContext) {
                return executor.executeChild(workflowPlan, *parentContext);
            }

            return executor.executeRoot(workflowPlan, task, executionOptions);
        }
        catch (...) {
            state->setException(std::current_exception());
            return {};
        }
    });

    auto* watcher = new QFutureWatcher<SharedWorkflowResult>();

    if (task != nullptr) {
        connect(watcher, &QFutureWatcher<SharedWorkflowResult>::finished, task, [watcher, task]() {
            const auto result = watcher->result();

            task->setProgress(1.0f);
            task->setFinished();
        },
        Qt::QueuedConnection);
    }

    connect(watcher, &QFutureWatcher<WorkflowResult>::finished, watcher,&QObject::deleteLater, Qt::QueuedConnection);

    watcher->setFuture(state->future);

	state->watcher = watcher;

    return WorkflowResultFuture(state);
}

SharedWorkflowResult WorkflowPlanExecutor::executeOnCurrentThread(WorkflowPlan& workflowPlan, Task* task, WorkflowExecutionOptions executionOptions /*= {}*/)
{
    SharedWorkflowResult result;

    if (auto* currentContext = WorkflowExecutionContext::current())
        result = executeChild(workflowPlan, *currentContext);
    else
        result = executeRoot(workflowPlan, task, executionOptions);

    return result;
}

SharedWorkflowResult WorkflowPlanExecutor::executeRoot(const WorkflowPlan& workflowPlan, Task* task, WorkflowExecutionOptions executionOptions /*= {}*/)
{
    QElapsedTimer elapsedTimer;

    elapsedTimer.start();

    auto rootContext = WorkflowExecutionContext::makeRoot(workflowPlan.getName(), task, executionOptions);

	WorkflowExecutionScope rootScope(rootContext);

    WorkflowReporter::info("Workflow started", workflowPlan.getName());

#ifdef USE_WORKFLOW_CONSOLE_TRACE_SINK
    trace(WorkflowTraceEvent{
        ._type = WorkflowTraceEventType::WorkflowStarted,
        ._name = workflowPlan.getName(),
        ._contextId = rootContext.getId(),
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
        executeImpl(workflowPlan);
        
        WorkflowReporter::info("Workflow finished", workflowPlan.getName());

#ifdef USE_WORKFLOW_CONSOLE_TRACE_SINK
        trace(WorkflowTraceEvent{
	        ._type = WorkflowTraceEventType::WorkflowFinished,
	        ._name = workflowPlan.getName(),
	        ._contextId = rootContext.getId(),
	        ._threadId = QThread::currentThreadId(),
	        ._timestampNs = AbstractWorkflowTraceSink::currentTimestampNs()
        });
#endif
    }
    catch (const ManiVaultException& exception) {
        WorkflowReporter::message(exception._severity, exception._message, workflowPlan.getName(), exception._code, exception._scope, exception._details);

        displayFailure(exception._message);

        if (exception._severity == SeverityLevel::Fatal)
            throw;
    }
    catch (const std::exception& exception) {
        WorkflowReporter::error(QString("Workflow failed: %1").arg(QString::fromUtf8(exception.what())), workflowPlan.getName());

        displayFailure(QString::fromUtf8(exception.what()));
        throw;
    }
    catch (...) {
        WorkflowReporter::error("Workflow failed with unknown error", workflowPlan.getName());
        displayFailure("Unknown error");

        throw;
    }

    auto result = std::make_shared<WorkflowResult>(workflowPlan.getName());

    if (auto state = rootContext.getState()) {
	    result->setMetrics(state->metrics().snapshot());
    	result->setMessages(WorkflowExecutionContext::current()->getState()->collectMessages());
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

            if (!message.isEmpty()) {
	            help().addNotification(title, message);
                qDebug() << title;
            }

    	}, Qt::QueuedConnection);
    }

    return result;
}

SharedWorkflowResult WorkflowPlanExecutor::executeChild(const WorkflowPlan& workflowPlan, WorkflowExecutionContext& parentContext)
{
    auto childContext = parentContext.createChild(
        workflowPlan.getName(),
        workflowPlan.getWeight(),
        WorkflowPlan::JobProgressMode::Automatic);

    WorkflowExecutionScope childScope(childContext);

    WorkflowReporter::info("Nested workflow started", workflowPlan.getName());

#ifdef USE_WORKFLOW_CONSOLE_TRACE_SINK
    trace({
	    ._type = WorkflowTraceEventType::WorkflowStarted,
	    ._name = workflowPlan.getName(),
	    ._contextId = childContext.getId(),
	    ._parentContextId = childContext.getParentId(),
	    ._metadata = {
	        { "nested", true }
	    }
    });
#endif

    executeImpl(workflowPlan);

    WorkflowReporter::info("Nested workflow finished", workflowPlan.getName());
    
#ifdef USE_WORKFLOW_CONSOLE_TRACE_SINK
    trace({
	    ._type = WorkflowTraceEventType::WorkflowFinished,
	    ._name = workflowPlan.getName(),
	    ._contextId = childContext.getId(),
	    ._parentContextId = childContext.getParentId(),
	    ._metadata = {
	        { "nested", true }
	    }
    });
#endif

    return {};
}

void WorkflowPlanExecutor::executeImpl(const WorkflowPlan& workflowPlan)
{
    std::exception_ptr primaryException;
    bool mainSucceeded = false;

    try {
        executeStageGroup(workflowPlan.getStages());
        mainSucceeded = true;
    }
    catch (...) {
        primaryException = std::current_exception();
    }

    try {
        if (mainSucceeded) {
            executeStageGroup(workflowPlan.getOnSuccessStages());
        }
        else {
            executeStageGroup(workflowPlan.getOnFailureStages());
        }
    }
    catch (...) {
        if (!primaryException)
            primaryException = std::current_exception();
    }

    try {
        executeStageGroup(workflowPlan.getFinallyStages());
    }
    catch (...) {
        if (!primaryException)
            primaryException = std::current_exception();
    }

    if (primaryException)
        std::rethrow_exception(primaryException);
}

void WorkflowPlanExecutor::executeStageGroup(const WorkflowPlan::Stages& stages)
{
    const auto stageCount = stages.size();

    if (stageCount == 0)
        return;

    auto* currentContext = WorkflowExecutionContext::current();
    if (!currentContext)
        throw std::runtime_error("No active workflow execution context");

    QVector<WorkflowExecutionContext> stageContexts;
    stageContexts.reserve(stageCount);

    for (int i = 0; i < stageCount; ++i) {
        const auto& stage = stages[i];
        stageContexts.push_back(currentContext->createChild(stage.getName(), stage.getWeight()));
    }

    for (int i = 0; i < stageCount; ++i) {

        const auto& stage = stages[i];
        auto& ctx = stageContexts[i];

        try {
            executeStage(stage, ctx);
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

void WorkflowPlanExecutor::executeStage(const WorkflowPlan::Stage& stage, WorkflowExecutionContext& stageContext)
{
    if (stage.getConcurrencyMode() == WorkflowPlan::ConcurrencyMode::Parallel && stage.containsGuiThreadJobs()) {
        throw ManiVaultException(SeverityLevel::Error, QString("Parallel stage '%1' contains GUI-thread jobs, which is not allowed.").arg(stage.getName()), "workflow.stage.parallel_gui_jobs", stage.getName());
    }

    WorkflowExecutionScope stageScope(stageContext);

    WorkflowReporter::info("Stage started", stage.getName());

    auto state = stageContext.getState();

#ifdef USE_WORKFLOW_CONSOLE_TRACE_SINK
    trace({
	    ._type = WorkflowTraceEventType::StageStarted,
	    ._name = stage.getName(),
	    ._contextId = stageContext.getId(),
	    ._parentContextId = stageContext.getParentId(),
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

        auto* currentContext = WorkflowExecutionContext::current();

        if (currentContext != nullptr) {
            const auto state = currentContext->getState();

            if (state && !state->getExecutionOptions()._parallel) {
                WorkflowReporter::warning("Parallel execution is disabled, switching to sequential mode", stage.getName());
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

#ifdef USE_WORKFLOW_CONSOLE_TRACE_SINK
        trace({
		    ._type = WorkflowTraceEventType::StageFinished,
		    ._name = stage.getName(),
		    ._contextId = stageContext.getId(),
		    ._parentContextId = stageContext.getParentId()
        });
#endif

    }
    catch (const ManiVaultException& exception) {
        handleStageException(stage, exception);

#ifdef USE_WORKFLOW_CONSOLE_TRACE_SINK
        trace({
            ._type = WorkflowTraceEventType::StageFinished,
            ._name = stage.getName(),
            ._contextId = stageContext.getId(),
            ._parentContextId = stageContext.getParentId()
        });
#endif
    }
    catch (const std::exception& exception) {
        WorkflowReporter::error(QString("Stage failed: %1").arg(exception.what()), stage.getName());
        
#ifdef USE_WORKFLOW_CONSOLE_TRACE_SINK
        trace({
            ._type = WorkflowTraceEventType::StageFinished,
            ._name = stage.getName(),
            ._contextId = stageContext.getId(),
            ._parentContextId = stageContext.getParentId()
        });
#endif

        throw;
    }
    catch (...) {
        WorkflowReporter::error("Stage failed with unknown error", stage.getName());

#ifdef USE_WORKFLOW_CONSOLE_TRACE_SINK
        trace({
            ._type = WorkflowTraceEventType::StageFinished,
            ._name = stage.getName(),
            ._contextId = stageContext.getId(),
            ._parentContextId = stageContext.getParentId()
        });
#endif

        throw;
    }
}

void WorkflowPlanExecutor::executeSequentialJobs(const WorkflowPlan::Stage& stage, WorkflowExecutionContext& stageContext)
{
#ifdef WORKFLOW_PLAN_EXECUTOR_VERBOSE
    qDebug() << "Executing sequential jobs for stage:" << stage.getName() << "in thread" << QThread::currentThread();
#endif

    const auto& jobs = stage.getJobs();
    const auto jobCount = jobs.size();

    if (jobCount == 0)
        return;

    QVector<WorkflowExecutionContext> jobContexts;
    jobContexts.reserve(jobCount);

    for (int jobIndex = 0; jobIndex < jobCount; ++jobIndex) {
        const auto& job = jobs[jobIndex];
        jobContexts.push_back(stageContext.createChild(job.getName(), job.getWeight(), job.getProgressMode()));
    }

    for (int jobIndex = 0; jobIndex < jobCount; ++jobIndex) {
        auto& job = const_cast<WorkflowPlan::Job&>(jobs[jobIndex]);
        auto& jobContext = jobContexts[jobIndex];

        executeJob(job, jobContext);
    }

    for (auto& jobContext : jobContexts) {
        const auto pendingCount = jobContext.getPendingAsyncWorkCount();

        if (pendingCount == 0)
            continue;

#ifdef USE_WORKFLOW_CONSOLE_TRACE_SINK
        trace({
            ._type = WorkflowTraceEventType::PendingAsyncWorkWaitStarted,
            ._name = jobContext.getName(),
            ._contextId = jobContext.getId(),
            ._parentContextId = jobContext.getParentId(),
            ._metadata = {
                { "pendingAsyncWorkCount", pendingCount }
            }
        });
#endif

        jobContext.waitForPendingAsyncWork();

#ifdef USE_WORKFLOW_CONSOLE_TRACE_SINK
        trace({
            ._type = WorkflowTraceEventType::PendingAsyncWorkWaitFinished,
            ._name = jobContext.getName(),
            ._contextId = jobContext.getId(),
            ._parentContextId = jobContext.getParentId()
        });
#endif
    }
}

void WorkflowPlanExecutor::executeParallelJobs(
    const WorkflowPlan::Stage& stage,
    WorkflowExecutionContext& stageContext)
{
#ifdef WORKFLOW_PLAN_EXECUTOR_VERBOSE
    qDebug() << "Executing parallel jobs for stage:"
        << stage.getName()
        << "in thread"
        << QThread::currentThread();
#endif

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
        ._contextId = stageContext.getId(),
        ._parentContextId = stageContext.getParentId(),
        ._metadata = {
            { "jobCount", jobCount },
            { "maxThreadCount", getThreadPool().maxThreadCount() },
            { "activeThreadCount", getThreadPool().activeThreadCount() }
        }
    });
#endif

    QVector<WorkflowExecutionContext> jobContexts;
    jobContexts.reserve(jobCount);

    for (int i = 0; i < jobCount; ++i) {
        const auto& job = jobs[i];

        jobContexts.push_back(
            stageContext.createChild(
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

        const auto stageNameCopy    = stage.getName();
        const auto jobIndexCopy     = jobIndex;

#ifdef USE_WORKFLOW_CONSOLE_TRACE_SINK
        trace({
            ._type = WorkflowTraceEventType::ParallelJobSubmitted,
            ._name = job.getName(),
            ._contextId = jobContext.getId(),
            ._parentContextId = jobContext.getParentId(),
            ._metadata = {
                { "stage", stage.getName() },
                { "jobIndex", jobIndex },
                { "jobCount", jobCount },
                { "maxThreadCount", getThreadPool().maxThreadCount() },
                { "activeThreadCount", getThreadPool().activeThreadCount() }
            }
        });
#endif

        synchronizer.addFuture(QtConcurrent::run(
            &getThreadPool(),
            [this,
            job,
            jobContext,
            stageName = stage.getName(),
            jobIndex = jobIndex,
            jobCount,
            &exceptionMutex,
            &firstException]() mutable {
                WorkflowExecutionScope jobScope(jobContext);

#ifdef USE_WORKFLOW_CONSOLE_TRACE_SINK
                trace({
                    ._type = WorkflowTraceEventType::ParallelJobStarted,
                    ._name = job.getName(),
                    ._contextId = jobContext.getId(),
                    ._parentContextId = jobContext.getParentId(),
                    ._metadata = {
                        { "stage", stageName },
                        { "jobIndex", jobIndex },
                        { "jobCount", jobCount },
                        { "maxThreadCount", getThreadPool().maxThreadCount() },
                        { "activeThreadCount", getThreadPool().activeThreadCount() }
                    }
                });
#endif

                try {
                    executeJob(job, jobContext);

#ifdef USE_WORKFLOW_CONSOLE_TRACE_SINK
                    trace({
                        ._type = WorkflowTraceEventType::ParallelJobFinished,
                        ._name = job.getName(),
                        ._contextId = jobContext.getId(),
                        ._parentContextId = jobContext.getParentId(),
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
                        ._contextId = jobContext.getId(),
                        ._parentContextId = jobContext.getParentId(),
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

    if (firstException) {
#ifdef USE_WORKFLOW_CONSOLE_TRACE_SINK
        trace({
            ._type = WorkflowTraceEventType::ParallelStageFailed,
            ._name = stage.getName(),
            ._contextId = stageContext.getId(),
            ._parentContextId = stageContext.getParentId()
        });
#endif

        std::rethrow_exception(firstException);
    }

#ifdef USE_WORKFLOW_CONSOLE_TRACE_SINK
    trace({
        ._type = WorkflowTraceEventType::ParallelStageFinished,
        ._name = stage.getName(),
        ._contextId = stageContext.getId(),
        ._parentContextId = stageContext.getParentId(),
        ._metadata = {
            { "jobCount", jobCount },
            { "maxThreadCount", getThreadPool().maxThreadCount() },
            { "activeThreadCount", getThreadPool().activeThreadCount() }
        }
    });
#endif
}

void WorkflowPlanExecutor::executeJobOnGuiThread(mv::util::WorkflowPlan::Job job, WorkflowExecutionContext& jobContext)
{
    auto& dispatcher = Application::workflowGuiThreadDispatcher();

    std::exception_ptr exceptionPtr;

    auto runOnGuiThread = [this, &job, &jobContext, &exceptionPtr]() {
        try {
            WorkflowExecutionScope scope(jobContext);

#ifdef USE_WORKFLOW_CONSOLE_TRACE_SINK
            trace({
			    ._type = WorkflowTraceEventType::GuiDispatchEntered,
			    ._name = job.getName(),
			    ._contextId = jobContext.getId(),
			    ._parentContextId = jobContext.getParentId()
            });
#endif

            job.run();
        }
        catch (...) {
            exceptionPtr = std::current_exception();
        }
        };

    if (QThread::currentThread() == dispatcher.thread()) {
        runOnGuiThread();
    }
    else {
#ifdef USE_WORKFLOW_CONSOLE_TRACE_SINK
        trace({
		    ._type = WorkflowTraceEventType::GuiDispatchRequested,
		    ._name = job.getName(),
		    ._contextId = jobContext.getId(),
		    ._parentContextId = jobContext.getParentId()
        });
#endif

        QMetaObject::invokeMethod(
            &dispatcher,
            runOnGuiThread,
            Qt::BlockingQueuedConnection
        );
    }

    if (exceptionPtr)
        std::rethrow_exception(exceptionPtr);
}

void WorkflowPlanExecutor::executeJobOnWorkerThread(mv::util::WorkflowPlan::Job job, WorkflowExecutionContext& jobContext)
{
#ifdef WORKFLOW_PLAN_EXECUTOR_VERBOSE
    qDebug() << "Executing job on worker thread:" << job.getName() << "in thread" << QThread::currentThread();
#endif

    WorkflowExecutionScope scope(jobContext);

    job.run();
}

void WorkflowPlanExecutor::executeJob(mv::util::WorkflowPlan::Job job, WorkflowExecutionContext& jobContext)
{
    WorkflowReporter::info("Job started", job.getName());

#ifdef USE_WORKFLOW_CONSOLE_TRACE_SINK
    trace({
	    ._type = WorkflowTraceEventType::JobStarted,
	    ._name = job.getName(),
	    ._contextId = jobContext.getId(),
	    ._parentContextId = jobContext.getParentId()
    });
#endif

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
            jobContext.setProgress(1.0);
        }
        else if (job.getProgressMode() == WorkflowPlan::JobProgressMode::Automatic &&
            !jobContext.hasProgressChildren()) {
            jobContext.setProgress(1.0);
        }

        WorkflowReporter::info("Job finished", job.getName());

#ifdef USE_WORKFLOW_CONSOLE_TRACE_SINK
        trace({
		    ._type = WorkflowTraceEventType::JobFinished,
		    ._name = job.getName(),
		    ._contextId = jobContext.getId(),
		    ._parentContextId = jobContext.getParentId()
        });
#endif
    }
    catch (...) {
        if (job.getProgressMode() == WorkflowPlan::JobProgressMode::Atomic || (job.getProgressMode() == WorkflowPlan::JobProgressMode::Automatic && !jobContext.hasProgressChildren())) {
            jobContext.setProgress(1.0);
        }

#ifdef USE_WORKFLOW_CONSOLE_TRACE_SINK
        trace({
		    ._type = WorkflowTraceEventType::JobFinished,
		    ._name = job.getName(),
		    ._contextId = jobContext.getId(),
		    ._parentContextId = jobContext.getParentId()
        });
#endif
        throw;
    }
}

void WorkflowPlanExecutor::handleStageException(const WorkflowPlan::Stage& stage, const ManiVaultException& exception)
{
    WorkflowReporter::message(exception._severity, exception._message, stage.getName(), exception._code, exception._scope, exception._details);

    if (exception._severity == SeverityLevel::Error || exception._severity == SeverityLevel::Fatal)
        throw;
}

