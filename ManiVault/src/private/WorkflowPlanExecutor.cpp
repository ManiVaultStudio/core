// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "WorkflowPlanExecutor.h"
#include "Taskflow.h"

#include <workflow/WorkflowResultRegistry.h>
#include <workflow/WorkflowMetric.h>

#include <CoreInterface.h>
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
using namespace mv::workflow;

void testTaskflow()
{
    tf::Executor executor;
    tf::Taskflow taskflow;

    auto A = taskflow.emplace([] {
        qDebug() << "A" << QThread::currentThread();
        });

    auto B = taskflow.emplace([] {
        qDebug() << "B" << QThread::currentThread();
        });

    auto C = taskflow.emplace([] {
        qDebug() << "C" << QThread::currentThread();
        });

    A.precede(C);
    B.precede(C);

    executor.run(taskflow).wait();
}

WorkflowPlanExecutor::WorkflowPlanExecutor(QObject* parent) :
	AbstractWorkflowPlanExecutor(parent)
{
    testTaskflow();
}

WorkflowResultFuture WorkflowPlanExecutor::execute(UniqueWorkflowPlan workflowPlan, SharedWorkflowExecutionContext parentContext /*= nullptr*/, OptionalWorkflowOptions options /*= std::nullopt*/)
{
    if (parentContext != nullptr) {
        const auto pendingWorkLabel = QString("Async workflow: %1").arg(workflowPlan->getName());
        const auto resolvedOptions  = options.value_or(parentContext->getState()->getOptions());

        auto childContext   = parentContext->createNestedWorkflowChild(workflowPlan->getName(), workflowPlan->getWeight(), WorkflowPlan::JobProgressMode::Automatic);
        auto future         = executeAsyncImpl(std::move(workflowPlan), resolvedOptions.reporting.progress ? Task::GuiScope::Background : Task::GuiScope::None, resolvedOptions, childContext);

        return future;
    }

    const auto resolvedOptions = options.value_or(WorkflowOptions{});

    return executeAsyncImpl(
        std::move(workflowPlan),
        resolvedOptions.reporting.progress ? Task::GuiScope::Background : Task::GuiScope::None,
        resolvedOptions,
        nullptr
    );
}

WorkflowResultFuture WorkflowPlanExecutor::executeAsyncImpl(UniqueWorkflowPlan workflowPlan, Task::GuiScope guiScope, const WorkflowOptions& options, SharedWorkflowExecutionContext executionContext)
{
    Q_UNUSED(workflowPlan)
    Q_UNUSED(guiScope)
    Q_UNUSED(options)
    Q_UNUSED(executionContext)

    return {};
}

SharedWorkflowResult WorkflowPlanExecutor::executeRoot(WorkflowPlan& workflowPlan, Task* task, const WorkflowOptions& options /*= {}*/)
{
    Q_UNUSED(workflowPlan)
    Q_UNUSED(task)
    Q_UNUSED(options)

    return {};
}

SharedWorkflowResult WorkflowPlanExecutor::executeChild(WorkflowPlan& workflowPlan, SharedWorkflowExecutionContext parentContext)
{
    Q_UNUSED(workflowPlan)
    Q_UNUSED(parentContext)

    return {};
}

void WorkflowPlanExecutor::executeJobOnGuiThread(const WorkflowPlan::Job& job, SharedWorkflowExecutionContext jobContext)
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

void WorkflowPlanExecutor::executeJobOnWorkerThread(const WorkflowPlan::Job& job, SharedWorkflowExecutionContext jobContext)
{
#ifdef WORKFLOW_PLAN_EXECUTOR_VERBOSE
    qDebug() << "Executing job on worker thread:" << job.getName() << "in thread" << QThread::currentThread();
#endif

    jobContext = requireContext(jobContext, __FUNCTION__);

    job.run(jobContext);
}

void WorkflowPlanExecutor::executeJob(const WorkflowPlan::Job& job, SharedWorkflowExecutionContext jobContext)
{
    jobContext->info(QString("Job started: %1").arg(job.getName()));

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

        jobContext->info(QString("Job finished: %1").arg(job.getName()));

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
    stageContext->message(exception.getSeverity(), exception.getMessage(), stage.getName(), exception.getDetails());

    if (exception.getSeverity() == SeverityLevel::Error || exception.getSeverity() == SeverityLevel::Fatal)
        throw;
}

