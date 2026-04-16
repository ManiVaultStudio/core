// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Workflow.h"

#ifdef _DEBUG
	#define WORKFLOW_VERBOSE
#endif

namespace mv::util
{

Workflow::Workflow(WorkflowPlan workflowPlan, SharedOperationContext operationContext /*= {}*/) :
    _operationContext(operationContext ? std::move(operationContext) : std::make_shared<OperationContext>()),
    _workflowPlan(std::move(workflowPlan))
{
#ifdef WORKFLOW_VERBOSE
    printLine(_title, "Create");
#endif
}

QString Workflow::getTitle() const
{
	return _workflowPlan.getTitle();
}

void Workflow::beginRun()
{
#ifdef WORKFLOW_VERBOSE
    printLine(_title, "Start", 1);
#endif

    _elapsedTimer.start();
}

void Workflow::endRun()
{
    _result._duration = static_cast<quint64>(_elapsedTimer.elapsed());
    _result._success = true;
}

void Workflow::cancel()
{
#ifdef WORKFLOW_VERBOSE
    printLine(_title, "Cancel", 1);
#endif

	//_runner.cancel();
}

WorkflowResult Workflow::getResult() const
{
    return _result;
}

const std::shared_ptr<OperationContext>& Workflow::getConstOperationContext() const
{
	return _operationContext;
}

SharedOperationContext Workflow::getOperationContext()
{
	return _operationContext;
}

}
