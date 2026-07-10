// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

namespace mv::workflow
{

using MaxWorkerThreadCount = std::uint32_t;

/** Options for configuring the parallelization behavior of a workflow. */
struct CORE_EXPORT WorkflowParallelizationOptions
{
    bool                    parallel = true;                /** Whether to execute the workflow in parallel using multiple threads (if supported by the workflow plan) or sequentially on a single thread. */
    MaxWorkerThreadCount    maxWorkerThreadCount = 63;      /** The maximum number of worker threads to use for parallel execution. This is ignored if parallel is false. The default value of 63 allows for up to 64 threads total (including the main thread) on platforms with a maximum of 64 concurrent threads. */
};

}