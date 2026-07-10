// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

namespace mv::workflow
{

using MaxWorkerThreadCount = std::uint32_t;

/** Options that configure workflow job parallelization. */
struct CORE_EXPORT WorkflowParallelizationOptions
{
    bool                    parallel = true;                /**< Whether parallel execution is enabled */
    MaxWorkerThreadCount    maxWorkerThreadCount = 63;      /**< Maximum number of worker threads for parallel execution */
};

}
