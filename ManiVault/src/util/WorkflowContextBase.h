// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

struct CORE_EXPORT WorkflowContextBase
{
public:
    virtual ~WorkflowContextBase() = default;
};

using UniqueTemporaryDir = std::unique_ptr<QTemporaryDir>;
using UniqueWorkflowContext = std::unique_ptr<WorkflowContextBase>;
