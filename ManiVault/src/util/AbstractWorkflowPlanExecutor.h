// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowPlan.h"

#include <QObject>

namespace mv::util
{

class CORE_EXPORT AbstractWorkflowPlanExecutor : public QObject
{
public:

    virtual void execute(WorkflowPlan& workflowPlan) = 0;
    virtual void runStage(const WorkflowPlan::Stage& stage) = 0;
    virtual void runStageInSequence(const WorkflowPlan::Stage& stage) = 0;
    virtual void runStageInParallel(const WorkflowPlan::Stage& stage) = 0;
};

}