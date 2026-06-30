// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <workflow/AbstractWorkflowPlanExecutor.h>
#include <workflow/WorkflowPlan.h>
#include <workflow/WorkflowResult.h>

#include <exception/ManiVaultException.h>

namespace mv
{
    class Task;
}

/*
 *
 * @author T. Kroes
 */
class WorkflowPlanExecutor final : public mv::workflow::AbstractWorkflowPlanExecutor
{
public:

    WorkflowPlanExecutor(QObject* parent = nullptr);

    mv::workflow::WorkflowResultFuture execute(mv::workflow::UniqueWorkflowPlan workflowPlan, mv::workflow::SharedWorkflowExecutionContext parentContext = nullptr, mv::workflow::OptionalWorkflowExecutionOptions executionOptions = std::nullopt) override;

protected:
    mv::workflow::WorkflowResultFuture executeAsyncImpl(mv::workflow::UniqueWorkflowPlan workflowPlan, mv::Task::GuiScope guiScope, const mv::workflow::WorkflowExecutionOptions& executionOptions, mv::workflow::SharedWorkflowExecutionContext executionContext) override;
	//SharedWorkflowResult executeOnCurrentThread(WorkflowPlan& workflowPlan, mv::Task* task, const WorkflowExecutionOptions& executionOptions = {}) override;
 //   SharedWorkflowResult executeOnCurrentThread(WorkflowPlan& workflowPlan, mv::Task* task, SharedWorkflowExecutionContext parentContext = nullptr, OptionalWorkflowExecutionOptions executionOptions = std::nullopt) override;

private:
    mv::workflow::SharedWorkflowResult executeRoot(mv::workflow::WorkflowPlan& workflowPlan, mv::Task* task, const mv::workflow::WorkflowExecutionOptions& executionOptions = {}) override;
    mv::workflow::SharedWorkflowResult executeChild(mv::workflow::WorkflowPlan& workflowPlan, mv::workflow::SharedWorkflowExecutionContext parentContext) override;

private: // Execute individual jobs
	void executeJobOnGuiThread(const mv::workflow::WorkflowPlan::Job& job, mv::workflow::SharedWorkflowExecutionContext jobContext) override;
	void executeJobOnWorkerThread(const mv::workflow::WorkflowPlan::Job& job, mv::workflow::SharedWorkflowExecutionContext jobContext) override;
	void executeJob(const mv::workflow::WorkflowPlan::Job& job, mv::workflow::SharedWorkflowExecutionContext jobContext) override;

private: // Helpers

    static void handleStageException(const mv::workflow::WorkflowPlan::Stage& stage, const mv::ManiVaultException& exception, mv::workflow::SharedWorkflowExecutionContext stageContext);

};

