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

public: // Tracing

    /**
     * Installs a trace sink to receive workflow trace events emitted by this executor. If a trace sink is already installed, it will be replaced by the new one.
     * @param sink The trace sink to install for receiving workflow trace events. Pass nullptr to uninstall any existing trace sink.
     */
    void setTraceSink(std::shared_ptr<AbstractWorkflowTraceSink> sink);

    /**
     * Emits a workflow trace event to the installed trace sink, if any.
     * @param event The workflow trace event to emit to the trace sink.
     */
    void trace(WorkflowTraceEvent event);

protected:
    virtual WorkflowResultFuture executeAsyncImpl(WorkflowPlan workflowPlan, Task::GuiScope guiScope, WorkflowExecutionOptions executionOptions = {}) = 0;
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
    virtual void executeJobOnGuiThread(const WorkflowPlan::Job& job, WorkflowExecutionContext& jobContext) = 0;
    virtual void executeJobOnWorkerThread(const WorkflowPlan::Job& job, WorkflowExecutionContext& jobContext) = 0;
    virtual void executeJob(const WorkflowPlan::Job& job, WorkflowExecutionContext& jobContext) = 0;

private:
    std::shared_ptr<AbstractWorkflowTraceSink> _traceSink;

    friend class WorkflowPlan;
};

using SharedWorkflowPlanExecutor = std::shared_ptr<AbstractWorkflowPlanExecutor>;

}