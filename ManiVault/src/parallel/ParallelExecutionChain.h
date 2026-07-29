// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

#include "workflow/WorkflowHandle.h"
#include "workflow/WorkflowOptions.h"
#include "workflow/WorkflowResult.h"

#include <QString>

namespace mv
{

class CORE_EXPORT ParallelExecutionChain
{
public:
    template<typename Function>
    ParallelExecutionChain& run(const QString& name, Function&& function);

    template<typename Range, typename Function>
    ParallelExecutionChain& forEach(const QString& name, Range&& range, Function&& function);

    template<typename Range, typename Function>
    ParallelExecutionChain& map(const QString& name, Range&& range, Function&& function);

    workflow::WorkflowResult execute(const workflow::WorkflowOptions& options = {});

    workflow::WorkflowHandle executeAsync(const workflow::WorkflowOptions& options = {});
};

}
