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

    WorkflowResultFuture execute(UniqueWorkflowPlan workflowPlan, SharedWorkflowExecutionContext parentContext = nullptr, OptionalWorkflowExecutionOptions executionOptions = std::nullopt) override;

    void waitForWorkflowResultFuture(const WorkflowResultFuture& future, WorkflowPlan::Job& job);

protected:
    WorkflowResultFuture executeAsyncImpl(UniqueWorkflowPlan workflowPlan, mv::Task::GuiScope guiScope, const WorkflowExecutionOptions& executionOptions, SharedWorkflowExecutionContext executionContext) override;
	//SharedWorkflowResult executeOnCurrentThread(WorkflowPlan& workflowPlan, mv::Task* task, const WorkflowExecutionOptions& executionOptions = {}) override;
 //   SharedWorkflowResult executeOnCurrentThread(WorkflowPlan& workflowPlan, mv::Task* task, SharedWorkflowExecutionContext parentContext = nullptr, OptionalWorkflowExecutionOptions executionOptions = std::nullopt) override;

private:
    SharedWorkflowResult executeRoot(WorkflowPlan& workflowPlan, mv::Task* task, const WorkflowExecutionOptions& executionOptions = {}) override;
    SharedWorkflowResult executeChild(WorkflowPlan& workflowPlan, SharedWorkflowExecutionContext parentContext) override;

private: // Execute individual jobs
	void executeJobOnGuiThread(const WorkflowPlan::Job& job, SharedWorkflowExecutionContext jobContext) override;
	void executeJobOnWorkerThread(const WorkflowPlan::Job& job, SharedWorkflowExecutionContext jobContext) override;
	void executeJob(const WorkflowPlan::Job& job, SharedWorkflowExecutionContext jobContext) override;

private: // Helpers

    static void handleStageException(const WorkflowPlan::Stage& stage, const mv::ManiVaultException& exception, SharedWorkflowExecutionContext stageContext);

};

