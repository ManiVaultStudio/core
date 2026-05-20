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

    SharedWorkflowResult executeBlocking(WorkflowPlan &workflowPlan, SharedWorkflowExecutionContext parentContext = nullptr, OptionalWorkflowExecutionOptions executionOptions = std::nullopt) override;
    WorkflowResultFuture executeAsync(WorkflowPlan &workflowPlan, SharedWorkflowExecutionContext parentContext = nullptr, OptionalWorkflowExecutionOptions executionOptions = std::nullopt) override;

public: // Thread pool access

    QThreadPool& getThreadPool();
    const QThreadPool& getThreadPool() const;

protected:
    WorkflowResultFuture executeAsyncImpl(WorkflowPlan workflowPlan, mv::Task::GuiScope guiScope, WorkflowExecutionOptions executionOptions, SharedWorkflowExecutionContext executionContext) override;
	SharedWorkflowResult executeOnCurrentThread(WorkflowPlan& workflowPlan, mv::Task* task, WorkflowExecutionOptions executionOptions = {}) override;
    SharedWorkflowResult executeOnCurrentThread(WorkflowPlan &workflowPlan, mv::Task *task, SharedWorkflowExecutionContext parentContext = nullptr, OptionalWorkflowExecutionOptions executionOptions = std::nullopt) override;

private:
    SharedWorkflowResult executeRoot(const WorkflowPlan& workflowPlan, mv::Task* task, WorkflowExecutionOptions executionOptions = {}) override;
    SharedWorkflowResult executeChild(const WorkflowPlan& workflowPlan, SharedWorkflowExecutionContext parentContext) override;
    void executeImpl(const WorkflowPlan& workflowPlan) override;
    void executeStage(const WorkflowPlan::Stage& stage, SharedWorkflowExecutionContext stageContext) override;
    void executeStageGroup(const WorkflowPlan::Stages& stages) override;

private: // Execute jobs in a stage
    void executeSequentialJobs(const WorkflowPlan::Stage& stage, SharedWorkflowExecutionContext stageContext) override;
    void executeParallelJobs(const WorkflowPlan::Stage& stage, SharedWorkflowExecutionContext stageContext) override;

private: // Execute individual jobs
	void executeJobOnGuiThread(WorkflowPlan::Job& job, SharedWorkflowExecutionContext jobContext) override;
	void executeJobOnWorkerThread(WorkflowPlan::Job& job, SharedWorkflowExecutionContext jobContext) override;
	void executeJob(WorkflowPlan::Job& job, SharedWorkflowExecutionContext jobContext) override;

private: // Helpers

    static void handleStageException(const WorkflowPlan::Stage& stage, const mv::ManiVaultException& exception);

};

