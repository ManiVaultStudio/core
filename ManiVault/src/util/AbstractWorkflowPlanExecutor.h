// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowPlan.h"
#include "WorkflowExecutionContext.h"
#include "WorkflowResult.h"
#include "Task.h"

#include <QObject>
#include <QElapsedTimer>

namespace mv::util
{

class CORE_EXPORT AbstractWorkflowPlanExecutor : public QObject
{
public:

    using ProgressCallback = std::function<void(double)>;

public:

    [[nodiscard]] virtual WorkflowResult execute(WorkflowPlan& workflowPlan, bool showProgress, WorkflowExecutionOptions executionOptions = {}) = 0;
    [[nodiscard]] virtual WorkflowResultFuture executeAsync(WorkflowPlan& workflowPlan, bool showProgress, WorkflowExecutionOptions executionOptions = {}) = 0;

protected:
    virtual WorkflowResultFuture executeAsyncImpl(WorkflowPlan workflowPlan, Task::GuiScope guiScope, WorkflowExecutionOptions executionOptions = {}) = 0;
    virtual WorkflowResult executeOnCurrentThread(WorkflowPlan& workflowPlan, Task* task = nullptr, WorkflowExecutionOptions executionOptions = {}) = 0;

    QElapsedTimer getElapsedTimer() const {
        return _elapsedTimer;
    }

    virtual void beginTimer() {
        _elapsedTimer.start();
    }

    virtual void endTimer(WorkflowResult& result) {
        result.setDuration(static_cast<quint64>(_elapsedTimer.elapsed()));
    }

private:
    virtual WorkflowResult executeRoot(const WorkflowPlan& workflowPlan, Task* task, WorkflowExecutionOptions executionOptions = {}) = 0;
    virtual WorkflowResult executeChild(const WorkflowPlan& workflowPlan, WorkflowExecutionContext& parentContext) = 0;
    virtual void executeImpl(const WorkflowPlan& workflowPlan) = 0;
    virtual void executeStage(const WorkflowPlan::Stage& stage, WorkflowExecutionContext& stageContext) = 0;

private: // Execute jobs in a stage
    virtual void executeSequentialJobs(const WorkflowPlan::Stage& stage, WorkflowExecutionContext& stageContext) = 0;
    virtual void executeParallelJobs(const WorkflowPlan::Stage& stage, WorkflowExecutionContext& stageContext) = 0;

private: // Execute individual jobs
    virtual void executeJobOnGuiThread(const WorkflowPlan::Job& job, WorkflowExecutionContext& jobContext) = 0;
    virtual void executeJobOnWorkerThread(const WorkflowPlan::Job& job, WorkflowExecutionContext& jobContext) = 0;
    virtual void executeJob(const WorkflowPlan::Job& job, WorkflowExecutionContext& jobContext) = 0;

private:
    QElapsedTimer   _elapsedTimer;

    friend class WorkflowPlan;
};

using SharedWorkflowPlanExecutor = std::shared_ptr<AbstractWorkflowPlanExecutor>;

}