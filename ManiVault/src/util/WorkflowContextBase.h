// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

class CORE_EXPORT WorkflowContextBase
{
public:
    virtual ~WorkflowContextBase() = default;

    QString     _errorMessage;
};

using UniqueWorkflowContext = std::unique_ptr<WorkflowContextBase>;
using SharedWorkflowContext = std::shared_ptr<WorkflowContextBase>;
