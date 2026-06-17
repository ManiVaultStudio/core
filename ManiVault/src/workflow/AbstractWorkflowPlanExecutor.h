// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowPlan.h"
#include "WorkflowExecutionContext.h"
#include "WorkflowResult.h"
#include "WorkflowResultFuture.h"
#include "Task.h"

#include <QObject>

namespace mv::workflow
{

class CORE_EXPORT AbstractWorkflowPlanExecutor : public QObject
{
public:

    AbstractWorkflowPlanExecutor(QObject* parent = nullptr);

    [[nodiscard]] virtual WorkflowResultFuture execute(UniqueWorkflowPlan workflowPlan, SharedWorkflowExecutionContext parentContext = nullptr, OptionalWorkflowExecutionOptions executionOptions = std::nullopt) = 0;
    [[nodiscard]] virtual SharedWorkflowResult executeBlocking(UniqueWorkflowPlan workflowPlan, Task* task = nullptr, WorkflowExecutionOptions executionOptions = {}) = 0;
    [[nodiscard]] virtual SharedWorkflowResult executeBlocking(UniqueWorkflowPlan workflowPlan, SharedWorkflowExecutionContext parentContext) = 0;

    static SharedWorkflowResult waitForAsync(WorkflowResultFuture::State& state);

	static void installNotificationLinkHandler();

protected:
    [[nodiscard]] virtual WorkflowResultFuture executeAsyncImpl(UniqueWorkflowPlan workflowPlan, Task::GuiScope guiScope, const WorkflowExecutionOptions& executionOptions, SharedWorkflowExecutionContext executionContext) = 0;
    //[[nodiscard]] virtual SharedWorkflowResult executeOnCurrentThread(WorkflowPlan& workflowPlan, Task* task, const WorkflowExecutionOptions& executionOptions = {}) = 0;
    //[[nodiscard]] virtual SharedWorkflowResult executeOnCurrentThread(WorkflowPlan& workflowPlan, Task* task, SharedWorkflowExecutionContext parentContext = nullptr, OptionalWorkflowExecutionOptions executionOptions = std::nullopt) = 0;

private:
    [[nodiscard]] virtual SharedWorkflowResult executeRoot(WorkflowPlan& workflowPlan, Task* task, const WorkflowExecutionOptions& executionOptions = {}) = 0;
    [[nodiscard]] virtual SharedWorkflowResult executeChild(WorkflowPlan& workflowPlan, SharedWorkflowExecutionContext parentContext) = 0;

private: // Execute individual jobs
    virtual void executeJobOnGuiThread(const WorkflowPlan::Job& job, SharedWorkflowExecutionContext jobContext) = 0;
    virtual void executeJobOnWorkerThread(const WorkflowPlan::Job& job, SharedWorkflowExecutionContext jobContext) = 0;
    virtual void executeJob(const WorkflowPlan::Job& job, SharedWorkflowExecutionContext jobContext) = 0;

protected:

    static SharedWorkflowExecutionContext requireContext(const SharedWorkflowExecutionContext& context, const char* where);

private:
    friend class WorkflowPlan;
};

using SharedWorkflowPlanExecutor = std::shared_ptr<AbstractWorkflowPlanExecutor>;
using UniqueWorkflowPlanExecutor = std::unique_ptr<AbstractWorkflowPlanExecutor>;

}