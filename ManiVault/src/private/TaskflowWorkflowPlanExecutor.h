// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "Taskflow.h"

#include <util/AbstractWorkflowPlanExecutor.h>
#include <util/WorkflowPlan.h>
#include <util/WorkflowResult.h>

#include <exception/ManiVaultException.h>

#include <QString>
#include <QThreadPool>

namespace mv
{
    class Task;
}

/*
 *
 * @author T. Kroes
 */
class TaskflowWorkflowPlanExecutor final : public AbstractWorkflowPlanExecutor
{
public:

    TaskflowWorkflowPlanExecutor(QObject* parent = nullptr);
    WorkflowResultFuture execute(UniqueWorkflowPlan workflowPlan, SharedWorkflowExecutionContext parentContext = nullptr, OptionalWorkflowExecutionOptions executionOptions = std::nullopt) override;

protected:
    WorkflowResultFuture executeAsyncImpl(UniqueWorkflowPlan workflowPlan, mv::Task::GuiScope guiScope, const WorkflowExecutionOptions& executionOptions, SharedWorkflowExecutionContext executionContext) override;

private:
    SharedWorkflowResult executeRoot(WorkflowPlan& workflowPlan, mv::Task* task, const WorkflowExecutionOptions& executionOptions = {}) override;
    SharedWorkflowResult executeChild(WorkflowPlan& workflowPlan, SharedWorkflowExecutionContext parentContext) override;
    void executeImpl(WorkflowPlan& workflowPlan, SharedWorkflowExecutionContext executionContext) override;
    void executeStage(const WorkflowPlan::Stage& stage, SharedWorkflowExecutionContext stageContext) override;
    void executeStageGroup(const WorkflowPlan::Stages& stages, SharedWorkflowExecutionContext executionContext) override;

private: // Execute jobs in a stage
    void executeSequentialJobs(const WorkflowPlan::Stage& stage, SharedWorkflowExecutionContext stageContext) override;
    void executeParallelJobs(const WorkflowPlan::Stage& stage, SharedWorkflowExecutionContext stageContext) override;

private: // Execute individual jobs
	void executeJobOnGuiThread(const WorkflowPlan::Job& job, SharedWorkflowExecutionContext jobContext) override;
	void executeJobOnWorkerThread(const WorkflowPlan::Job& job, SharedWorkflowExecutionContext jobContext) override;
	void executeJob(const WorkflowPlan::Job& job, SharedWorkflowExecutionContext jobContext) override;

private: // Helpers

    static void handleStageException(const WorkflowPlan::Stage& stage, const mv::ManiVaultException& exception, SharedWorkflowExecutionContext stageContext);

    void compileWorkflow(
        const WorkflowPlan& workflowPlan,
        tf::Taskflow& taskflow,
        SharedWorkflowExecutionContext parentContext);

    void compileWorkflow(
        const WorkflowPlan& workflowPlan,
        tf::Subflow& subflow,
        SharedWorkflowExecutionContext parentContext);

private:
    tf::Executor    _executor;  
};

