// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowExecutionState.h"

#include <QString>

namespace mv::util
{

class CORE_EXPORT WorkflowConsoleDashboard
{
public:
    explicit WorkflowConsoleDashboard(WorkflowExecutionState::Ptr state);

    void render();

private:
    WorkflowExecutionState::Ptr _state;
};

} // namespace mv::util