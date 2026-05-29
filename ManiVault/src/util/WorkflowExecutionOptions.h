// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "AbstractWorkflowTraceSink.h"

namespace mv::util
{

using MaxWorkerThreadCount = std::uint32_t;

struct CORE_EXPORT WorkflowExecutionOptions
{
    bool                            _parallel = true;                                                   /** Whether to execute the workflow in parallel using multiple threads (if supported by the workflow plan) or sequentially on a single thread. */
    MaxWorkerThreadCount            _maxWorkerThreadCount = 63;                                         /** The maximum number of worker threads to use for parallel execution. This is ignored if _parallel is false. The default value of 63 allows for up to 64 threads total (including the main thread) on platforms with a maximum of 64 concurrent threads. Adjust this value based on the expected workload and system capabilities. */
    bool                            _reportProgress = false;                                            /** Whether to report progress during workflow execution. */
    bool                            _addNotification = false;                                           /** Whether to add notifications during workflow execution. */
    SharedWorkflowTraceSink         _traceSink;                                                         /** Initialized based on _executionOptions._traceSinkType */
    bool                            _enableConsoleDashboard = false;                                    /** Whether to enable the console dashboard for this workflow execution. This will be ignored if a trace sink is provided, as the trace sink is responsible for managing its own dashboard if needed. */
    std::uint32_t                   _maxConsoleLogDepth = std::numeric_limits<std::uint32_t>::max();    /** The maximum depth of the workflow execution graph to log to the console */
};

/** Optional WorkflowExecutionOptions for configuring workflow execution behavior */
using OptionalWorkflowExecutionOptions = std::optional<WorkflowExecutionOptions>;

} // namespace mv::util