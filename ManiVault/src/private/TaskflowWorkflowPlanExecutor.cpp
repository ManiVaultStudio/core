// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "TaskflowWorkflowPlanExecutor.h"
#include "Taskflow.h"

#include <Task.h>

#include <util/WorkflowExecutionLifecycleScope.h>

#include <future>

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

        auto childContext = parentContext->createWorkflowChild(workflowPlan->getName(), workflowPlan->getWeight(), WorkflowPlan::JobProgressMode::Automatic);
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

SharedWorkflowResult TaskflowWorkflowPlanExecutor::executeBlocking(UniqueWorkflowPlan workflowPlan, mv::Task* task, WorkflowExecutionOptions executionOptions)
{
    return executeRoot(*workflowPlan, task, executionOptions);
}

SharedWorkflowResult TaskflowWorkflowPlanExecutor::executeBlocking(UniqueWorkflowPlan workflowPlan, SharedWorkflowExecutionContext parentContext)
{
    if (!workflowPlan)
        return {};

    if (parentContext)
        return executeChild(*workflowPlan, parentContext);

    return executeRoot(*workflowPlan, nullptr, {});
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

    state->future = std::async(std::launch::async, [this, state, workflowPlan = std::move(workflowPlan), task, executionOptions, executionContext]() mutable -> SharedWorkflowResult {
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
    auto rootContext = WorkflowExecutionContext::makeRoot(workflowPlan.getName(), task, executionOptions);

    std::optional<WorkflowConsoleDashboardScope> dashboardScope;

    if (rootContext->isRootExecution() && executionOptions._enableConsoleDashboard) {
        dashboardScope.emplace(rootContext->getState());
    }

    WorkflowExecutionLifecycleScope lifecycle(rootContext);

    const auto displayFailure = [&workflowPlan, executionOptions](const QString& message) {
        QMetaObject::invokeMethod(&help(), [&workflowPlan, executionOptions, message]() {
            const auto title = QString("%1 failed").arg(workflowPlan.getName());

            if (executionOptions._addNotification)
                help().addNotification(title, message);

            qDebug() << QString("%1: %2").arg(title, message);
            });
        };

    auto runStages = [this, rootContext](const WorkflowPlan::Stages& stages) {
        if (stages.empty())
            return;

        tf::Taskflow taskflow;

        compileStages(stages, taskflow, rootContext);

        if (taskflow.num_tasks() > 0)
            _executor.run(taskflow).get();
        };

    std::exception_ptr primaryException;

    try {
        tf::Taskflow taskflow;

        compileWorkflow(workflowPlan, taskflow, rootContext);

        if (taskflow.num_tasks() > 0)
            _executor.run(taskflow).get();
    }
    catch (...) {
        primaryException = std::current_exception();
    }

    if (primaryException) {
        try {
            std::rethrow_exception(primaryException);
        }
        catch (const ManiVaultException& exception) {
            rootContext->error(exception._message, exception._where,exception._details);

            lifecycle.fail(exception._message);

            displayFailure(exception._message);

            try {
                runStages(workflowPlan.getOnFailureStages());
            }
            catch (const std::exception& failureException) {
                rootContext->error(QString("Failure stages failed: %1").arg(QString::fromUtf8(failureException.what())), workflowPlan.getName());
            }
            catch (...) {
                rootContext->error("Failure stages failed with unknown error", workflowPlan.getName());
            }

            if (exception._severity == SeverityLevel::Fatal)
                std::rethrow_exception(primaryException);
        }
        catch (const std::exception& exception) {
            const auto message = QString::fromUtf8(exception.what());

            rootContext->error(message, workflowPlan.getName());

            lifecycle.fail(message);

            displayFailure(QString::fromUtf8(exception.what()));

            try {
                runStages(workflowPlan.getOnFailureStages());
            }
            catch (const std::exception& failureException) {
                rootContext->error(QString("Failure stages failed: %1").arg(QString::fromUtf8(failureException.what())), workflowPlan.getName());
            }
            catch (...) {
                rootContext->error("Failure stages failed with unknown error", workflowPlan.getName());
            }

            std::rethrow_exception(primaryException);
        }
        catch (...) {
            rootContext->error("Workflow failed with unknown error", workflowPlan.getName());

        	lifecycle.fail("Workflow failed with unknown error");

            displayFailure("Unknown error");

            try {
                runStages(workflowPlan.getOnFailureStages());
            }
            catch (const std::exception& failureException) {
                rootContext->error(QString("Failure stages failed: %1").arg(QString::fromUtf8(failureException.what())), workflowPlan.getName());
            }
            catch (...) {
                rootContext->error("Failure stages failed with unknown error", workflowPlan.getName());
            }

            std::rethrow_exception(primaryException);
        }
    }

    try {
        runStages(workflowPlan.getFinallyStages());
    }
    catch (const std::exception& exception) {
        rootContext->error(QString("Finally stages failed: %1").arg(QString::fromUtf8(exception.what())), workflowPlan.getName());

        if (!primaryException)
            throw;
    }
    catch (...) {
        rootContext->error(
            "Finally stages failed with unknown error",
            workflowPlan.getName());

        if (!primaryException)
            throw;
    }

    auto result = std::make_shared<WorkflowResult>(workflowPlan.getName());

    if (auto state = rootContext->getState()) {
        result->setValue(rootContext->takeResultValues());
        result->setMetrics(state->metrics().snapshot());
        result->setMessages(state->collectMessages());
        result->setDuration(lifecycle.elapsedMs());

        for (const auto& message : state->collectMessages()) {
            qDebug() << "###Message:" << message._text << "Level:" << getSeverityLevelName(message._level) << "Emitter:" << message._emitter;
        }
    }

    if (rootContext->getState()->getExecutionOptions()._addNotification) {
        addWorkflowFinishedNotification(
            workflowPlan.getName(),
            result,
            WorkflowResultRegistry::instance().add(result));
    }

    lifecycle.finish();

    return result;
}

SharedWorkflowResult TaskflowWorkflowPlanExecutor::executeChild(WorkflowPlan& workflowPlan, SharedWorkflowExecutionContext parentContext)
{
    auto childContext = parentContext->createNestedWorkflowChild(workflowPlan.getName(), workflowPlan.getWeight(), WorkflowPlan::JobProgressMode::Automatic);

    WorkflowExecutionLifecycleScope lifecycle(childContext);

    try {
        tf::Taskflow taskflow;

        compileWorkflow(workflowPlan, taskflow, childContext);

        if (taskflow.num_tasks() > 0)
            _executor.run(taskflow).get();

        lifecycle.finish();
    }
    catch (const std::exception& e) {
        const auto message = QString::fromUtf8(e.what());
        lifecycle.fail(message);
        throw;
    }
    catch (...) {
        lifecycle.fail(QStringLiteral("Unknown nested workflow error"));
        throw;
    }

    auto result = std::make_shared<WorkflowResult>(workflowPlan.getName());
    result->setValue(childContext->takeResultValues());

    return result;
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
        std::promise<void> promise;
        auto future = promise.get_future();

        QMetaObject::invokeMethod(
            qApp,
            [runOnGuiThread = std::move(runOnGuiThread),
            promise = std::move(promise)]() mutable
            {
                runOnGuiThread();
                promise.set_value();
            },
            Qt::QueuedConnection
        );

        future.wait();
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
    jobContext = requireContext(jobContext, __FUNCTION__);

    WorkflowExecutionLifecycleScope lifecycle(jobContext);

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
    }
    catch (const std::exception& e) {
        jobContext->reportFailed(QString::fromUtf8(e.what()));
        throw;
    }
    catch (...) {
        jobContext->reportFailed("Unknown exception");
        throw;
    }
}

void TaskflowWorkflowPlanExecutor::handleStageException(const WorkflowPlan::Stage& stage, const ManiVaultException& exception, SharedWorkflowExecutionContext stageContext)
{
    stageContext->message(exception._severity, exception._message, stage.getName(), exception._details);

    if (exception._severity == SeverityLevel::Error || exception._severity == SeverityLevel::Fatal)
        throw;
}

TaskflowWorkflowPlanExecutor::CompiledTasks TaskflowWorkflowPlanExecutor::compileWorkflow(const WorkflowPlan& workflowPlan, tf::Taskflow& taskflow, SharedWorkflowExecutionContext parentContext)
{
    return compileWorkflowImpl(workflowPlan, taskflow, parentContext);
}

TaskflowWorkflowPlanExecutor::CompiledTasks TaskflowWorkflowPlanExecutor::compileWorkflow(const WorkflowPlan& workflowPlan, tf::Subflow& subflow, SharedWorkflowExecutionContext parentContext)
{
    return compileWorkflowImpl(workflowPlan, subflow, parentContext);
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
    jobContext = requireContext(jobContext, __FUNCTION__);

    if (!job.isNestedWorkflow()) {
        executeJob(job, jobContext);
        return;
    }

    auto childPlan = job.createNestedWorkflow(jobContext);

    if (!childPlan)
        throw std::runtime_error("Nested workflow job returned null workflow plan");

    auto childContext = jobContext->createNestedWorkflowChild(
        childPlan->getName(),
        childPlan->getWeight(),
        WorkflowPlan::JobProgressMode::Automatic);

    WorkflowExecutionLifecycleScope lifecycle(childContext);

    compileWorkflow(*childPlan, subflow, childContext);

    subflow.join();

    lifecycle.finish();
}

void TaskflowWorkflowPlanExecutor::runStagesRoot(const WorkflowPlan::Stages& stages, SharedWorkflowExecutionContext rootContext)
{
	if (stages.empty())
		return;

	tf::Taskflow taskflow;

	compileStages(stages, taskflow, rootContext);

	if (taskflow.num_tasks() > 0)
		_executor.run(taskflow).get();
}

