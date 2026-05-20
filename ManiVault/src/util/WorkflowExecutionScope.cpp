// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowExecutionScope.h"
#include "WorkflowExecutionContext.h"

namespace mv::util
{

WorkflowExecutionScope::WorkflowExecutionScope(SharedWorkflowExecutionContext context) :
	_previous(WorkflowExecutionContext::current()),
	_context(std::move(context))
{
	WorkflowExecutionContext::setCurrent(_context);
}

WorkflowExecutionScope::~WorkflowExecutionScope()
{
	WorkflowExecutionContext::setCurrent(_previous);
}

}
