// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowConsoleDashboardScope.h"

namespace mv::util
{

WorkflowConsoleDashboardScope::WorkflowConsoleDashboardScope(WorkflowExecutionState::Ptr state)
{
	_dashboard = std::make_unique<WorkflowConsoleDashboard>(std::move(state));
	_dashboard->start();
}

WorkflowConsoleDashboardScope::~WorkflowConsoleDashboardScope()
{
	_dashboard->stop();
}

}
