// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

#include <QString>

class CORE_EXPORT WorkflowResultBase
{
public:
    virtual ~WorkflowResultBase() = default;

    bool        _success = false;
    QString     _errorMessage;
};

using UniqueWorkflowResultBase = std::unique_ptr<WorkflowResultBase>;