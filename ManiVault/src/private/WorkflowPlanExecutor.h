// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

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
class WorkflowPlanExecutor final : public AbstractWorkflowPlanExecutor
{
public:

    WorkflowPlanExecutor(QObject* parent = nullptr);

	SharedWorkflowResult executeBlocking(WorkflowPlan& workflowPlan, WorkflowExecutionOptions executionOptions) override;
    SharedWorkflowResult executeBlocking(WorkflowPlan& workflowPlan, WorkflowExecutionOptions executionOptions, std::optional<WorkflowExecutionContext> parentContext) override;
	WorkflowResultFuture executeAsync(WorkflowPlan& workflowPlan, WorkflowExecutionOptions executionOptions) override;
    WorkflowResultFuture executeAsync(WorkflowPlan& workflowPlan, WorkflowExecutionOptions executionOptions, std::optional<WorkflowExecutionContext> parentContext) override;

public: // Thread pool access

    QThreadPool& getThreadPool();
    const QThreadPool& getThreadPool() const;

protected:
    WorkflowResultFuture executeAsyncImpl(WorkflowPlan workflowPlan, mv::Task::GuiScope guiScope, WorkflowExecutionOptions executionOptions, std::optional<WorkflowExecutionContext> executionContext) override;
	SharedWorkflowResult executeOnCurrentThread(WorkflowPlan& workflowPlan, mv::Task* task, WorkflowExecutionOptions executionOptions = {}) override;
    SharedWorkflowResult executeOnCurrentThread(WorkflowPlan& workflowPlan, mv::Task* task, WorkflowExecutionOptions executionOptions, std::optional<WorkflowExecutionContext> parentContext) override;

private:
    SharedWorkflowResult executeRoot(const WorkflowPlan& workflowPlan, mv::Task* task, WorkflowExecutionOptions executionOptions = {}) override;
    SharedWorkflowResult executeChild(const WorkflowPlan& workflowPlan, WorkflowExecutionContext& parentContext) override;
    void executeImpl(const WorkflowPlan& workflowPlan) override;
    void executeStage(const WorkflowPlan::Stage& stage, WorkflowExecutionContext& stageContext) override;
    void executeStageGroup(const WorkflowPlan::Stages& stages) override;

private: // Execute jobs in a stage
    void executeSequentialJobs(const WorkflowPlan::Stage& stage, WorkflowExecutionContext& stageContext) override;
    void executeParallelJobs(const WorkflowPlan::Stage& stage, WorkflowExecutionContext& stageContext) override;

private: // Execute individual jobs
	void executeJobOnGuiThread(WorkflowPlan::Job job, WorkflowExecutionContext& jobContext) override;
	void executeJobOnWorkerThread(WorkflowPlan::Job job, WorkflowExecutionContext& jobContext) override;
	void executeJob(WorkflowPlan::Job job, WorkflowExecutionContext& jobContext) override;

private: // Helpers

    static void handleStageException(const WorkflowPlan::Stage& stage, const mv::ManiVaultException& exception);

};

