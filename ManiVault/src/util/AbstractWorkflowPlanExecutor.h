// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowPlan.h"
#include "WorkflowExecutionContext.h"
#include "WorkflowResult.h"
#include "AbstractWorkflowTraceSink.h"
#include "Task.h"

#include <QObject>

namespace mv::util
{

class CORE_EXPORT AbstractWorkflowPlanExecutor : public QObject
{
public:

    AbstractWorkflowPlanExecutor(QObject* parent = nullptr);

    [[nodiscard]] virtual SharedWorkflowResult executeBlocking(WorkflowPlan& workflowPlan, WorkflowExecutionOptions executionOptions = {}) = 0;
    [[nodiscard]] virtual WorkflowResultFuture executeAsync(WorkflowPlan& workflowPlan, WorkflowExecutionOptions executionOptions = {}) = 0;

    static void installNotificationLinkHandler();

protected:
    virtual WorkflowResultFuture executeAsyncImpl(WorkflowPlan workflowPlan, Task::GuiScope guiScope, WorkflowExecutionOptions executionOptions, std::optional<WorkflowExecutionContext> parentContext) = 0;
    virtual SharedWorkflowResult executeOnCurrentThread(WorkflowPlan& workflowPlan, Task* task = nullptr, WorkflowExecutionOptions executionOptions = {}) = 0;

private:
    virtual SharedWorkflowResult executeRoot(const WorkflowPlan& workflowPlan, Task* task, WorkflowExecutionOptions executionOptions = {}) = 0;
    virtual SharedWorkflowResult executeChild(const WorkflowPlan& workflowPlan, WorkflowExecutionContext& parentContext) = 0;
    virtual void executeImpl(const WorkflowPlan& workflowPlan) = 0;
    virtual void executeStage(const WorkflowPlan::Stage& stage, WorkflowExecutionContext& stageContext) = 0;
    virtual void executeStageGroup(const WorkflowPlan::Stages& stages) = 0;

private: // Execute jobs in a stage
    virtual void executeSequentialJobs(const WorkflowPlan::Stage& stage, WorkflowExecutionContext& stageContext) = 0;
    virtual void executeParallelJobs(const WorkflowPlan::Stage& stage, WorkflowExecutionContext& stageContext) = 0;

private: // Execute individual jobs
    virtual void executeJobOnGuiThread(WorkflowPlan::Job job, WorkflowExecutionContext& jobContext) = 0;
    virtual void executeJobOnWorkerThread(WorkflowPlan::Job job, WorkflowExecutionContext& jobContext) = 0;
    virtual void executeJob(WorkflowPlan::Job, WorkflowExecutionContext& jobContext) = 0;

protected: // Tracing

    /**
     * Traces a workflow event by sending it to the trace sink associated with the current workflow execution context, if available. The event's thread ID and timestamp are automatically set before tracing.
     * @param event The WorkflowTraceEvent object containing details about the event to be traced.
     */
    static void trace(WorkflowTraceEvent event);

private:
    friend class WorkflowPlan;
};

using SharedWorkflowPlanExecutor = std::shared_ptr<AbstractWorkflowPlanExecutor>;

}