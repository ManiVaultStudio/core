// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <util/AbstractWorkflowPlanExecutor.h>
#include <util/WorkflowPlan.h>
#include <util/WorkflowResult.h>

#include <QString>
#include <QThreadPool>

namespace mv
{
    class Task;
}

/*
 * TaskTreeWorkflowPlanExecutor is a concrete implementation of the AbstractWorkflowPlanExecutor interface that executes a workflow plan using a task tree workflow. It creates a SerializePlanWorkflow with the provided workflow plan and starts it, waiting for the workflow to complete. If the workflow finishes with an error, it throws a runtime error with the error message. The workflow is stored as a unique pointer, which ensures that it is properly cleaned up when it is no longer needed. The workflow will be created in the execute method when the workflow plan is executed, and it will be reset to nullptr when the workflow is done.
 *
 * @author T. Kroes
 */
class WorkflowPlanExecutor final : public mv::util::AbstractWorkflowPlanExecutor
{
public:

    WorkflowPlanExecutor(QObject* parent = nullptr);

	mv::util::SharedWorkflowResult execute(mv::util::WorkflowPlan& workflowPlan, mv::util::WorkflowExecutionOptions executionOptions = {}) override;
	mv::util::WorkflowResultFuture executeAsync(mv::util::WorkflowPlan& workflowPlan, mv::util::WorkflowExecutionOptions executionOptions = {}) override;

    QThreadPool& getThreadPool();
    const QThreadPool& getThreadPool() const;

protected:
    mv::util::SharedWorkflowResult executeOnCurrentThread(mv::util::WorkflowPlan& workflowPlan, mv::Task* task = nullptr, mv::util::WorkflowExecutionOptions executionOptions = {}) override;
    mv::util::WorkflowResultFuture executeAsyncImpl(mv::util::WorkflowPlan workflowPlan, mv::Task::GuiScope guiScope, mv::util::WorkflowExecutionOptions executionOptions = {}) override;

private:
    mv::util::SharedWorkflowResult executeRoot(const mv::util::WorkflowPlan& workflowPlan, mv::Task* task, mv::util::WorkflowExecutionOptions executionOptions = {}) override;
    mv::util::SharedWorkflowResult executeChild(const mv::util::WorkflowPlan& workflowPlan, mv::util::WorkflowExecutionContext& parentContext) override;
    void                     executeImpl(const mv::util::WorkflowPlan& workflowPlan) override;
    void                     executeStage(const mv::util::WorkflowPlan::Stage& stage, mv::util::WorkflowExecutionContext& stageContext) override;

private: // Execute jobs in a stage
    void executeSequentialJobs(const mv::util::WorkflowPlan::Stage& stage, mv::util::WorkflowExecutionContext& stageContext) override;
    void executeParallelJobs(const mv::util::WorkflowPlan::Stage& stage, mv::util::WorkflowExecutionContext& stageContext) override;

private: // Execute individual jobs
	void executeJobOnGuiThread(const mv::util::WorkflowPlan::Job& job, mv::util::WorkflowExecutionContext& jobContext) override;
	void executeJobOnWorkerThread(const mv::util::WorkflowPlan::Job& job, mv::util::WorkflowExecutionContext& jobContext);
	void executeJob(const mv::util::WorkflowPlan::Job& job, mv::util::WorkflowExecutionContext& jobContext) override;
};

