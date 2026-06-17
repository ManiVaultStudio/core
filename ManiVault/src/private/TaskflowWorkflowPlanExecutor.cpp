// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "TaskflowWorkflowPlanExecutor.h"
#include "Taskflow.h"

#include <CoreInterface.h>
#include <Application.h>

#include <Task.h>

#include <workflow/WorkflowExecutionContext.h>
#include <workflow/WorkflowConsoleDashboardScope.h>
#include <workflow/WorkflowExecutionLifecycleScope.h>
#include <workflow/WorkflowResultRegistry.h>

#include <util/Miscellaneous.h>

#include <QCoreApplication>
#include <QEventLoop>
#include <QThread>
#include <QDateTime>

#include <fstream>
#include <chrono>
#include <future>

#ifdef _DEBUG
    //#define WORKFLOW_PLAN_EXECUTOR_VERBOSE
#endif

using namespace mv;
using namespace mv::util;
using namespace mv::workflow;

TaskflowWorkflowPlanExecutor::TaskflowWorkflowPlanExecutor(QObject* parent) :
    AbstractWorkflowPlanExecutor(parent)
{
}

WorkflowResultFuture TaskflowWorkflowPlanExecutor::execute(
    UniqueWorkflowPlan workflowPlan,
    SharedWorkflowExecutionContext parentContext,
    OptionalWorkflowExecutionOptions executionOptions)
{
    if (parentContext != nullptr) {
        const auto resolvedOptions =
            executionOptions.value_or(parentContext->getState()->getExecutionOptions());

        auto childContext = parentContext->createWorkflowChild(workflowPlan->getName(), workflowPlan->getWeight(), WorkflowPlan::JobProgressMode::Automatic);

        return executeAsyncImpl(std::move(workflowPlan), resolvedOptions._reportProgress ? Task::GuiScope::Modal : Task::GuiScope::None, resolvedOptions, childContext);
    }

    const auto resolvedOptions = executionOptions.value_or({});

    return executeAsyncImpl(std::move(workflowPlan), resolvedOptions._reportProgress ? Task::GuiScope::Modal : Task::GuiScope::None, resolvedOptions, nullptr);
}

SharedWorkflowResult TaskflowWorkflowPlanExecutor::executeBlocking(
    UniqueWorkflowPlan workflowPlan,
    mv::Task* task,
    WorkflowExecutionOptions executionOptions)
{
    return executeRoot(*workflowPlan, task, executionOptions);
}

SharedWorkflowResult TaskflowWorkflowPlanExecutor::executeBlocking(
    UniqueWorkflowPlan workflowPlan,
    SharedWorkflowExecutionContext parentContext)
{
    static thread_local int depth = 0;

    ++depth;

    Q_ASSERT(depth < 50);

    if (!workflowPlan)
        throw std::runtime_error("Workflow plan is null");

    SharedWorkflowExecutionContext context;

    if (parentContext) {
        context = parentContext->createNestedWorkflowChild(
            workflowPlan->getName(),
            workflowPlan->getWeight(),
            WorkflowPlan::JobProgressMode::Automatic);

        Q_ASSERT(context);
        Q_ASSERT(context->getState() == parentContext->getState());
    }
    else {
        context = WorkflowExecutionContext::makeRoot(
            workflowPlan->getName(),
            nullptr,
            {});
    }

    return executeWithContext(*workflowPlan, context);
}

WorkflowResultFuture TaskflowWorkflowPlanExecutor::executeAsyncImpl(
    UniqueWorkflowPlan workflowPlan,
    Task::GuiScope guiScope,
    const WorkflowExecutionOptions& executionOptions,
    SharedWorkflowExecutionContext executionContext)
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

    state->future = std::async(
        std::launch::async,
        [this, state, workflowPlan = std::move(workflowPlan), task, executionOptions, executionContext]() mutable -> SharedWorkflowResult {
            try {
                if (executionContext != nullptr)
                    return executeWithContext(*workflowPlan, executionContext);

                return executeRoot(*workflowPlan, task, executionOptions);
            }
            catch (...) {
                state->setException(std::current_exception());
                return {};
            }
        });

    return WorkflowResultFuture(state);
}

SharedWorkflowResult TaskflowWorkflowPlanExecutor::executeRoot(WorkflowPlan& workflowPlan, Task* task, const WorkflowExecutionOptions& executionOptions)
{
    //_chromeObserver.reset();
    //_chromeObserver = _executor.make_observer<tf::ChromeObserver>();

    auto rootContext = WorkflowExecutionContext::makeRoot(workflowPlan.getName(), task, executionOptions);

    return executeWithContext(workflowPlan, rootContext);
}

SharedWorkflowResult TaskflowWorkflowPlanExecutor::executeWithContext(WorkflowPlan& workflowPlan, SharedWorkflowExecutionContext rootContext)
{
    rootContext = requireContext(rootContext, __FUNCTION__);

    if (rootContext->isRootExecution() && _chromeObserver) {

        if (_chromeObserver) {
            _chromeObserver.reset();
        } else {
            _chromeObserver = _executor.make_observer<tf::ChromeObserver>();
        }
    }

    const auto executionOptions = rootContext->getState() ? rootContext->getState()->getExecutionOptions() : WorkflowExecutionOptions{};

    std::optional<WorkflowConsoleDashboardScope> dashboardScope;

    if (rootContext->isRootExecution() && executionOptions._enableConsoleDashboard)
        dashboardScope.emplace(rootContext->getState());

    WorkflowExecutionLifecycleScope lifecycle(rootContext);

    auto runStages = [this, rootContext](const WorkflowPlan::Stages& stages) {
        if (stages.empty())
            return;

        tf::Taskflow taskflow;

        compileStages(stages, taskflow, rootContext);

        if (taskflow.num_tasks() > 0)
            runTaskflowBlocking(taskflow);
        };

    std::exception_ptr primaryException;

    try {
        tf::Taskflow taskflow;

        compileWorkflow(workflowPlan, taskflow, rootContext);

        if (taskflow.num_tasks() > 0)
            runTaskflowBlocking(taskflow);
    }
    catch (...) {
        primaryException = std::current_exception();
    }

    if (primaryException) {
        try {
            std::rethrow_exception(primaryException);
        }
        catch (const ManiVaultException& exception) {
            rootContext->error(exception._message, exception._where, exception._details);
            lifecycle.fail(exception._severity, exception._message, exception._details);

            try {
                runStages(workflowPlan.getOnFailureStages());
            }
            catch (const std::exception& failureException) {
                rootContext->error(
                    QString("Failure stages failed: %1").arg(QString::fromUtf8(failureException.what())),
                    workflowPlan.getName());
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
            lifecycle.fail(SeverityLevel::Error, message);

            try {
                runStages(workflowPlan.getOnFailureStages());
            }
            catch (const std::exception& failureException) {
                rootContext->error(
                    QString("Failure stages failed: %1").arg(QString::fromUtf8(failureException.what())),
                    workflowPlan.getName());
            }
            catch (...) {
                rootContext->error("Failure stages failed with unknown error", workflowPlan.getName());
            }

            //std::rethrow_exception(primaryException);
        }
        catch (...) {
            rootContext->error("Workflow failed with unknown error", workflowPlan.getName());
            lifecycle.fail(SeverityLevel::Error, "Workflow failed with unknown error");

            try {
                runStages(workflowPlan.getOnFailureStages());
            }
            catch (const std::exception& failureException) {
                rootContext->error(
                    QString("Failure stages failed: %1").arg(QString::fromUtf8(failureException.what())),
                    workflowPlan.getName());
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
        rootContext->error(
            QString("Finally stages failed: %1").arg(QString::fromUtf8(exception.what())),
            workflowPlan.getName());

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

    const auto durationMs = lifecycle.elapsedMs();

    if (auto state = rootContext->getState()) {
        if (rootContext->isRootExecution()) {
            const auto messages = state->collectMessages();
            result->setMessages(messages);
        }

        auto output = rootContext->takeOutput();

        if (output.isValid() && !output.isNull()) {
            result->setValue(output);
        }
        else {
            auto resultValues = state->takeResultValues(rootContext->getId());
            result->setValue(resultValues);
        }
        result->setMetrics(state->metrics().snapshot());
    }

    result->setDuration(durationMs);

    lifecycle.finish(durationMs);

    if (rootContext->isRootExecution()) {
        if (auto task = rootContext->getTask()) {
            QMetaObject::invokeMethod(task, [task]() {
                task->setProgress(1.0f);
                task->setStatus(Task::Status::Finished);
            }, Qt::QueuedConnection);
        }
    }

    if (rootContext->isRootExecution() && rootContext->getState() && rootContext->getState()->getExecutionOptions()._addNotification) {
        addWorkflowFinishedNotification(
            workflowPlan.getName(),
            result,
            WorkflowResultRegistry::instance().add(result));
    }

    if (rootContext->isRootExecution() && rootContext->getExecutionOptions()._profilingSinkType == WorkflowExecutionOptions::ProfilingSinkType::ChromeTracing) {
        const auto fileName = QString("profiling/workflow_trace_%1.json").arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));

        std::ofstream traceFile(fileName.toStdString());

        if (traceFile.is_open()) {
            _chromeObserver->dump(traceFile);
            qDebug() << "Chrome trace written to" << fileName;
        }
    }

    return result;
}

SharedWorkflowResult TaskflowWorkflowPlanExecutor::executeChild(
    WorkflowPlan& workflowPlan,
    SharedWorkflowExecutionContext parentContext)
{
    auto childContext = parentContext->createNestedWorkflowChild(
        workflowPlan.getName(),
        workflowPlan.getWeight(),
        WorkflowPlan::JobProgressMode::Automatic);

    Q_ASSERT(childContext);
    Q_ASSERT(childContext->getState() == parentContext->getState());

    return executeWithContext(workflowPlan, childContext);
}

void TaskflowWorkflowPlanExecutor::executeJobOnGuiThread(
    const WorkflowPlan::Job& job,
    SharedWorkflowExecutionContext jobContext)
{
    jobContext = requireContext(jobContext, __FUNCTION__);

    auto& dispatcher = Application::workflowGuiThreadDispatcher();

    auto exceptionPtr = std::make_shared<std::exception_ptr>();

    auto runOnGuiThread = [&job, jobContext, exceptionPtr]() mutable {
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
            promise = std::move(promise)]() mutable {
                runOnGuiThread();
                promise.set_value();
            },
            Qt::QueuedConnection);

        future.wait();
    }

    if (*exceptionPtr)
        std::rethrow_exception(*exceptionPtr);
}

void TaskflowWorkflowPlanExecutor::executeJobOnWorkerThread(
    const WorkflowPlan::Job& job,
    SharedWorkflowExecutionContext jobContext)
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
        else if (job.getProgressMode() == WorkflowPlan::JobProgressMode::Automatic &&
            !jobContext->hasProgressChildren()) {
            jobContext->setProgress(1.0);
        }

        lifecycle.finish();
    }
    catch (const std::exception& e) {
        jobContext->reportFailed(SeverityLevel::Error, QString::fromUtf8(e.what()));
        throw;
    }
    catch (...) {
        jobContext->reportFailed(SeverityLevel::Error, "Unknown exception");
        throw;
    }
}

void TaskflowWorkflowPlanExecutor::handleStageException(const WorkflowPlan::Stage& stage, const ManiVaultException& exception, SharedWorkflowExecutionContext stageContext)
{
    stageContext->message(
        exception._severity,
        exception._message,
        stage.getName(),
        exception._details);

    if (exception._severity == SeverityLevel::Error ||
        exception._severity == SeverityLevel::Fatal) {
        throw;
    }
}

TaskflowWorkflowPlanExecutor::CompiledTasks TaskflowWorkflowPlanExecutor::compileWorkflow(
    const WorkflowPlan& workflowPlan,
    tf::Taskflow& taskflow,
    SharedWorkflowExecutionContext parentContext)
{
    return compileWorkflowImpl(workflowPlan, taskflow, parentContext);
}

TaskflowWorkflowPlanExecutor::CompiledTasks TaskflowWorkflowPlanExecutor::compileWorkflow(
    const WorkflowPlan& workflowPlan,
    tf::Subflow& subflow,
    SharedWorkflowExecutionContext parentContext)
{
    return compileWorkflowImpl(workflowPlan, subflow, parentContext);
}

void TaskflowWorkflowPlanExecutor::addWorkflowFinishedNotification(const QString& workflowName, const SharedWorkflowResult& result, const QUuid& resultId)
{
    const auto url      = QString("app://workflow/results?workflowResultId=%1").arg(resultId.toString(QUuid::WithoutBraces));
    const auto title    = QString("%1 finished in %2").arg(workflowName).arg(getElapsedTimeHumanReadable(result->getDuration(), true));

    QMetaObject::invokeMethod(&help(), [result, url, title]() {
        QString message;

        if (!result->hasWarnings() && !result->hasErrors()) {
            message = QStringLiteral("Completed successfully");
        }

        if (result->hasWarnings() && !result->hasErrors()) {
            message = QString("Completed with <a href=\"%1\">warnings</a>. Review the report.").arg(QString("%1&levels=warning").arg(url));
        }

        if (!result->hasWarnings() && result->hasErrors()) {
            message = QString("Completed with <a href=\"%1\">errors</a>. Review the report.").arg(QString("%1&levels=error").arg(url));
        }

        if (result->hasWarnings() && result->hasErrors()) {
            message = QString("Completed with <a href=\"%1\">warnings</a> and <a href=\"%2\">errors</a>. Review the report.").arg(QString("%1&levels=warning").arg(url), QString("%1&levels=error").arg(url));
        }

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

    auto childContext = jobContext->createNestedWorkflowChild(
        job.getName(),
        job.getWeight(),
        job.getProgressMode());

    Q_ASSERT(childContext);
    Q_ASSERT(childContext->getState() == jobContext->getState());

    childContext->setOutputId(job.getOutputId());

    WorkflowExecutionLifecycleScope lifecycle(childContext);

    try {
        auto childPlan = job.createNestedWorkflow(childContext);

        if (!childPlan)
            throw std::runtime_error("Nested workflow job returned null workflow plan");

        compileWorkflow(*childPlan, subflow, childContext);

        subflow.join();

        if (job.getProgressMode() == WorkflowPlan::JobProgressMode::Atomic)
            jobContext->setProgress(1.0);

        lifecycle.finish();
    }
    catch (const ManiVaultException& maniVaultException) {
        childContext->reportFailed(maniVaultException._severity, maniVaultException._message, maniVaultException._details);
        throw;
    }
    catch (const std::exception& exception) {
        childContext->reportFailed(
            SeverityLevel::Error,
            QString::fromUtf8(exception.what()));
        throw;
    }
    catch (...) {
        childContext->reportFailed(
            SeverityLevel::Error,
            "Unknown exception");
        throw;
    }
}

WorkflowHandle TaskflowWorkflowPlanExecutor::getFinalStageHandle(const WorkflowPlan& workflowPlan)
{
	const auto& successStages = workflowPlan.getOnSuccessStages();
	if (!successStages.empty())
		return successStages.back().getHandle();

	const auto& stages = workflowPlan.getStages();
	if (!stages.empty())
		return stages.back().getHandle();

	return {};
}

void TaskflowWorkflowPlanExecutor::runStagesRoot(
    const WorkflowPlan::Stages& stages,
    SharedWorkflowExecutionContext rootContext)
{
    if (stages.empty())
        return;

    tf::Taskflow taskflow;

    compileStages(stages, taskflow, rootContext);

    if (taskflow.num_tasks() > 0)
        runTaskflowBlocking(taskflow);
}

void TaskflowWorkflowPlanExecutor::runTaskflowBlocking(tf::Taskflow& taskflow)
{
    if (taskflow.num_tasks() == 0)
        return;

    auto future = _executor.run(taskflow);

    if (QThread::currentThread() != qApp->thread()) {
        future.get();
        return;
    }

    while (future.wait_for(std::chrono::milliseconds(10)) != std::future_status::ready)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 10);

    future.get();
}

std::string TaskflowWorkflowPlanExecutor::makeTraceName(const QString& kind, const QString& name)
{
	return QString("%1 | %2").arg(kind, name).toStdString();
}
