// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

#include <QString>

namespace mv::util
{

class CORE_EXPORT WorkflowResult
{
public:
    virtual ~WorkflowResult() = default;

    bool            _success = false;
    QString         _errorMessage;
    std::uint64_t   _duration;
};

using UniqueWorkflowResult = std::unique_ptr<WorkflowResult>;

} // namespace mv::util