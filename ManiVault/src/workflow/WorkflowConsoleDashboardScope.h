// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowConsoleDashboard.h"

namespace mv::workflow
{

class CORE_EXPORT WorkflowConsoleDashboardScope
{
public:
    explicit WorkflowConsoleDashboardScope(WorkflowExecutionState::Ptr state);

    ~WorkflowConsoleDashboardScope();

private:
    std::unique_ptr<WorkflowConsoleDashboard> _dashboard;
};

}