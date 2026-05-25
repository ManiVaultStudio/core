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

    Task* task = nullptr;

    if (guiScope != Task::GuiScope::None) {
        task = new Task(nullptr, workflowPlan->getName());
        task->setGuiScopes({ guiScope });
        task->setStatus(Task::Status::Running);
        task->setProgress(0.0f);
    }

    state->task = task;

    state->future = _executor.async([this, state, workflowPlan = std::move(workflowPlan), task, executionOptions, executionContext]() mutable -> SharedWorkflowResult {
        try {
            if (executionContext != nullptr) {
                throw std::runtime_error("Nested workflow execution through executeAsyncImpl is no longer supported. Use addNestedWorkflowStage.");
            }

            return executeRoot(*workflowPlan, task, executionOptions);
        }
        catch (...) {
            state->setException(std::current_exception());
            return {};
        }
	});

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
        tf::Taskflow taskflow;

        compileWorkflow(workflowPlan, taskflow, rootContext);

        _executor.run(taskflow).wait();

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

    if (rootContext->getState()->getExecutionOptions()._addNotification)
		addWorkflowFinishedNotification(workflowPlan.getName(), result, WorkflowResultRegistry::instance().add(result));

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

    //executeImpl(workflowPlan, childContext);

    //WorkflowReporter::info("Nested workflow finished", workflowPlan.getName());

    return {};
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

void TaskflowWorkflowPlanExecutor::compileWorkflow(const WorkflowPlan& workflowPlan, tf::Taskflow& taskflow, SharedWorkflowExecutionContext parentContext)
{
    auto mainTasks = compileStages(workflowPlan.getStages(), taskflow, parentContext);

    auto successTasks = compileStages(workflowPlan.getOnSuccessStages(), taskflow, parentContext);

    for (auto& mainEnd : mainTasks.ends) {
        for (auto& successStart : successTasks.starts) {
            mainEnd.precede(successStart);
        }
    }

    auto finallyTasks = compileStages(workflowPlan.getFinallyStages(), taskflow, parentContext);

    for (auto& successEnd : successTasks.ends) {
        for (auto& finallyStart : finallyTasks.starts) {
            successEnd.precede(finallyStart);
        }
    }
}

void TaskflowWorkflowPlanExecutor::compileWorkflow(const WorkflowPlan& workflowPlan, tf::Subflow& subflow, SharedWorkflowExecutionContext parentContext)
{
    auto mainTasks = compileStages(
        workflowPlan.getStages(),
        subflow,
        parentContext);

    auto successTasks = compileStages(
        workflowPlan.getOnSuccessStages(),
        subflow,
        parentContext);

    for (auto& mainEnd : mainTasks.ends) {
        for (auto& successStart : successTasks.starts) {
            mainEnd.precede(successStart);
        }
    }

    auto finallyTasks = compileStages(
        workflowPlan.getFinallyStages(),
        subflow,
        parentContext);

    for (auto& successEnd : successTasks.ends) {
        for (auto& finallyStart : finallyTasks.starts) {
            successEnd.precede(finallyStart);
        }
    }
}

void TaskflowWorkflowPlanExecutor::addWorkflowFinishedNotification(const QString& workflowName, const SharedWorkflowResult& result, const QUuid& resultId)
{
    const auto url = QString("app://workflow/results?workflowResultId=%1").arg(resultId.toString(QUuid::WithoutBraces));

    const auto title = QString("%1 finished in %2")
        .arg(workflowName)
        .arg(getElapsedTimeHumanReadable(result->getDuration(), true));

    QMetaObject::invokeMethod(&help(), [result, url, title]() {
        QString message;

        if (!result->hasWarnings() && !result->hasErrors())
            message = QString("Completed successfully, see the <a href='%1'>report</a> for details").arg(QString("%1&levels=info").arg(url));

        if (result->hasWarnings() && !result->hasErrors())
            message = QString("Completed with <a href=\"%1\">warnings</a>. Review the report.").arg(QString("%1&levels=warning").arg(url));

        if (!result->hasWarnings() && result->hasErrors())
            message = QString("Completed with <a href=\"%1\">errors</a>. Review the report.").arg(QString("%1&levels=error").arg(url));

        if (result->hasWarnings() && result->hasErrors())
            message = QString("Completed with <a href=\"%1\">warnings </a> and <a href=\"%1\">errors </a>. Review the report.").arg(QString("%1&levels=warning,error,critical").arg(url));

        if (!message.isEmpty()) {
            help().addNotification(title, message);
            qDebug() << title;
        }
    }, Qt::QueuedConnection);
}

void TaskflowWorkflowPlanExecutor::executeCompiledJob(const WorkflowPlan::Job& job, tf::Subflow& subflow, SharedWorkflowExecutionContext jobContext)
{
    Q_UNUSED(subflow)

        if (job.isNestedWorkflow()) {
            jobContext->info(QString("Nested workflow job started: %1").arg(job.getName()));

            auto childPlan = job.createNestedWorkflow(jobContext);

            auto childContext = jobContext->createChild(
                childPlan->getName(),
                childPlan->getWeight(),
                WorkflowPlan::JobProgressMode::Automatic);

            compileWorkflow(*childPlan, subflow, childContext);
            return;
        }

    executeJob(job, jobContext);
}

