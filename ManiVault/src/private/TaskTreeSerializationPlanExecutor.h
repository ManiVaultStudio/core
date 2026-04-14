// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <util/AbstractWorkflowPlanExecutor.h>
#include <util/WorkflowPlan.h>

#include "SerializePlanWorkflow.h"

#include <QString>

class TaskTreeWorkflowPlanExecutor final : public mv::util::AbstractWorkflowPlanExecutor
{
public:

    void execute(mv::util::WorkflowPlan& workflowPlan) override;

private:
    std::unique_ptr<SerializePlanWorkflow>      _workflow;
};

