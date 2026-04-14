// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "WorkflowPlanExecutor.h"

#include <util/OperationContext.h>
#include <util/OperationContextScope.h>

#ifdef _DEBUG
	#define WORKFLOW_PLAN_EXECUTOR_VERBOSE
#endif

using namespace mv::util;

void WorkflowPlanExecutor::execute(WorkflowPlan& workflowPlan)
{
    _workflow = std::make_unique<Workflow>(
        workflowPlan,
        OperationContextScope::getShared()
    );

    //_workflow->setDoneCallback(
    //    [&](bool success, const QString& error) {
    //        if (!success) {
    //            errorMessage = error;
    //        }

    //        donePromise.set_value({ success, error });
    //    }
    //);

    _workflow->start();
}
