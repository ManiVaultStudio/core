// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "TaskflowWorkflowPlanExecutor.h"
#include "Taskflow.h"

#include <Task.h>

#include <QtConcurrent>

#ifdef _DEBUG
	//#define WORKFLOW_PLAN_EXECUTOR_VERBOSE
#endif

using namespace mv;
using namespace mv::util;

TaskflowWorkflowPlanExecutor::TaskflowWorkflowPlanExecutor(QObject* parent) :
	AbstractWorkflowPlanExecutor(parent)
{
}

WorkflowResultFuture TaskflowWorkflowPlanExecutor::execute(UniqueWorkflowPlan workflowPlan, SharedWorkflowExecutionContext parentContext /*= nullptr*/, OptionalWorkflowExecutionOptions executionOptions /*= std::nullopt*/)
{
    if (parentContext != nullptr) {
        const auto pendingWorkLabel = QString("Async workflow: %1").arg(workflowPlan->getName());
        const auto resolvedOptions = executionOptions.value_or(parentContext->getState()->getExecutionOptions());

        auto childContext = parentContext->createChild(workflowPlan->getName(), workflowPlan->getWeight(), WorkflowPlan::JobProgressMode::Automatic);
        auto future = executeAsyncImpl(std::move(workflowPlan), resolvedOptions._reportProgress ? Task::GuiScope::Background : Task::GuiScope::None, resolvedOptions, childContext);

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

WorkflowResultFuture TaskflowWorkflowPlanExecutor::executeAsyncImpl(UniqueWorkflowPlan workflowPlan, Task::GuiScope guiScope, const WorkflowExecutionOptions& executionOptions, SharedWorkflowExecutionContext executionContext)
{
    auto state = std::make_shared<WorkflowResultFuture::State>();

    auto* watcher = new QFutureWatcher<SharedWorkflowResult>();

    state->watcher = watcher;

    connect(watcher, &QFutureWatcher<SharedWorkflowResult>::finished, watcher, &QObject::deleteLater, Qt::QueuedConnection);

    Task* task = nullptr;

    if (guiScope != Task::GuiScope::None) {
        task = new Task(nullptr, workflowPlan->getName());
        task->setGuiScopes({ guiScope });
        task->setStatus(Task::Status::Running);
        task->setProgress(0.0f);
    }

    state->task = task;

    state->future = QtConcurrent::run([this, state, workflowPlan = std::move(workflowPlan), task, executionOptions, executionContext]() mutable -> SharedWorkflowResult {
        try {
            if (executionContext != nullptr) {
                //WorkflowReporter::info("Nested workflow started");

                executeImpl(*workflowPlan, executionContext);

                //WorkflowReporter::info("Nested workflow finished");

                return {};
            }

            return executeRoot(*workflowPlan, task, executionOptions);
        }
        catch (...) {
            state->setException(std::current_exception());
            return {};
        }
        });

    watcher->setFuture(state->future);

    return WorkflowResultFuture(state);
}

SharedWorkflowResult TaskflowWorkflowPlanExecutor::executeRoot(WorkflowPlan& workflowPlan, Task* task, const WorkflowExecutionOptions& executionOptions /*= {}*/)
{
    QElapsedTimer elapsedTimer;

    elapsedTimer.start();

    auto rootContext = WorkflowExecutionContext::makeRoot(workflowPlan.getName(), task, executionOptions);

    //WorkflowReporter::info("Workflow started", workflowPlan.getName());

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
        const auto url = QString("app://workflow/results?workflowResultId=%1").arg(resultId.toString(QUuid::WithoutBraces));
        const auto title = QString("%1 finished in %2").arg(workflowPlan.getName()).arg(getElapsedTimeHumanReadable(result->getDuration(), true));

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

SharedWorkflowResult TaskflowWorkflowPlanExecutor::executeChild(WorkflowPlan& workflowPlan, SharedWorkflowExecutionContext parentContext)
{
    auto childContext = parentContext->createChild(
        workflowPlan.getName(),
        workflowPlan.getWeight(),
        WorkflowPlan::JobProgressMode::Automatic);

    //WorkflowReporter::info("Nested workflow started", workflowPlan.getName());

    executeImpl(workflowPlan, childContext);

    //WorkflowReporter::info("Nested workflow finished", workflowPlan.getName());

    return {};
}

void TaskflowWorkflowPlanExecutor::executeImpl(WorkflowPlan& workflowPlan, SharedWorkflowExecutionContext executionContext)
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

void TaskflowWorkflowPlanExecutor::executeStageGroup(const WorkflowPlan::Stages& stages, SharedWorkflowExecutionContext executionContext)
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

void TaskflowWorkflowPlanExecutor::executeStage(const WorkflowPlan::Stage& stage, SharedWorkflowExecutionContext stageContext)
{
    stageContext = requireContext(stageContext, __FUNCTION__);

    if (stage.getConcurrencyMode() == WorkflowPlan::ConcurrencyMode::Parallel && stage.containsGuiThreadJobs()) {
        throw ManiVaultException(SeverityLevel::Error, QString("Parallel stage '%1' contains GUI-thread jobs, which is not allowed.").arg(stage.getName()), "workflow.stage.parallel_gui_jobs", stage.getName());
    }

    stageContext->info(QString("Stage started: %1").arg(stage.getName()));

    auto state = stageContext->getState();

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

        stageContext->info(QString("Stage finished: %1").arg(stage.getName()));
    }
    catch (const ManiVaultException& exception) {
        handleStageException(stage, exception, stageContext);
    }
    catch (const std::exception& exception) {
        stageContext->error(QString("Stage failed: %1").arg(exception.what()));

        throw;
    }
    catch (...) {
        stageContext->error("Stage failed with unknown error");

        throw;
    }
}

void TaskflowWorkflowPlanExecutor::executeSequentialJobs(const WorkflowPlan::Stage& stage, SharedWorkflowExecutionContext stageContext)
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

        jobContext->waitForPendingAsyncWork();
    }
}

void TaskflowWorkflowPlanExecutor::executeParallelJobs(const WorkflowPlan::Stage& stage, SharedWorkflowExecutionContext stageContext)
{
    tf::Taskflow taskflow;

    QVector<SharedWorkflowExecutionContext> jobContexts;

    for (const auto& job : stage.getJobs()) {
        jobContexts.push_back(
            stageContext->createChild(
                job.getName(),
                job.getWeight(),
                job.getProgressMode()));
    }

    for (int i = 0; i < stage.getJobs().size(); ++i) {
        auto job = stage.getJobs()[i];
        auto context = jobContexts[i];

        taskflow.emplace([this, job, context]() {
            executeJob(job, context);
        });
    }

    _executor.run(taskflow).wait();
}

void TaskflowWorkflowPlanExecutor::executeJobOnGuiThread(const WorkflowPlan::Job& job, SharedWorkflowExecutionContext jobContext)
{
    jobContext = requireContext(jobContext, __FUNCTION__);

    auto& dispatcher = Application::workflowGuiThreadDispatcher();

    auto exceptionPtr = std::make_shared<std::exception_ptr>();
    auto futurePtr = std::make_shared<std::optional<WorkflowResultFuture>>();

    auto runOnGuiThread = [&job, jobContext, exceptionPtr, futurePtr]() mutable {
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

void TaskflowWorkflowPlanExecutor::executeJobOnWorkerThread(const WorkflowPlan::Job& job, SharedWorkflowExecutionContext jobContext)
{
#ifdef WORKFLOW_PLAN_EXECUTOR_VERBOSE
    qDebug() << "Executing job on worker thread:" << job.getName() << "in thread" << QThread::currentThread();
#endif

    jobContext = requireContext(jobContext, __FUNCTION__);

    job.run(jobContext);
}

void TaskflowWorkflowPlanExecutor::executeJob(const WorkflowPlan::Job& job, SharedWorkflowExecutionContext jobContext)
{
    jobContext->info(QString("Job started: %1").arg(job.getName()));

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

        jobContext->info(QString("Job finished: %1").arg(job.getName()));
    }
    catch (...) {
        if (job.getProgressMode() == WorkflowPlan::JobProgressMode::Atomic || (job.getProgressMode() == WorkflowPlan::JobProgressMode::Automatic && !jobContext->hasProgressChildren())) {
            jobContext->setProgress(1.0);
        }

        throw;
    }
}

void TaskflowWorkflowPlanExecutor::handleStageException(const WorkflowPlan::Stage& stage, const ManiVaultException& exception, SharedWorkflowExecutionContext stageContext)
{
    stageContext->message(exception._severity, exception._message, stage.getName(), exception._details);

    if (exception._severity == SeverityLevel::Error || exception._severity == SeverityLevel::Fatal)
        throw;
}

