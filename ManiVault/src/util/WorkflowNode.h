// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QtTaskTree>

namespace mv::util
{

/** Type of workflow operation */
enum class WorkflowOperation
{
    Load,
    Save
};

/** Interface for workflow nodes that can generate a workflow for a given operation */
class CORE_EXPORT WorkflowNode
{
public:
    virtual ~WorkflowNode() = default;

    virtual QtTaskTree::Group makeWorkflow(WorkflowOperation workflowOperation) = 0;
};

}