// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowExecutionContext.h"

namespace mv::util
{

namespace
{
    thread_local WorkflowExecutionContext* currentWorkflowExecutionContext = nullptr;
}

WorkflowExecutionContext* WorkflowExecutionContext::current()
{
	return currentWorkflowExecutionContext;
}

const WorkflowExecutionContext* WorkflowExecutionContext::currentConst()
{
	return currentWorkflowExecutionContext;
}

void WorkflowExecutionContext::setCurrent(WorkflowExecutionContext* context)
{
    currentWorkflowExecutionContext = context;
}

}
