// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowBatchingOptions.h"

namespace mv::workflow
{

using MaxWorkerThreadCount = std::uint32_t;

struct CORE_EXPORT WorkflowExecutionOptions
{
    enum class ProfilingSinkType
    {
        None,               /** No profiling or tracing will be performed. */
        ChromeTracing       /** Profiling data will be collected and output in a format compatible with Chrome's tracing tools. */
    };

    bool                    parallel = true;                                                    /** Whether to execute the workflow in parallel using multiple threads (if supported by the workflow plan) or sequentially on a single thread. */
    MaxWorkerThreadCount    maxWorkerThreadCount = 63;                                          /** The maximum number of worker threads to use for parallel execution. This is ignored if parallel is false. The default value of 63 allows for up to 64 threads total (including the main thread) on platforms with a maximum of 64 concurrent threads. Adjust this value based on the expected workload and system capabilities. */
    bool                    reportProgress = false;                                             /** Whether to report progress during workflow execution. (with a modal task) */
    bool                    addNotification = false;                                            /** Whether to add notifications during workflow execution. */
    bool                    enableConsoleDashboard = false;                                     /** Whether to enable the console dashboard for this workflow execution. This will be ignored if a trace sink is provided, as the trace sink is responsible for managing its own dashboard if needed. */
    std::uint32_t           maxConsoleLogDepth = std::numeric_limits<std::uint32_t>::max();     /** The maximum depth of the workflow execution graph to log to the console */
    WorkflowBatchingOptions workflowBatchingOptions;                                            /** Batching options for workflow execution. These options control the degree of parallelism for various operations during workflow execution, such as loading and saving datasets, and encoding and decoding data blocks. Adjust these options based on the expected workload and system capabilities to achieve optimal performance without overwhelming system resources. (under normal circumstances this should be sufficient) */
    ProfilingSinkType       profilingSinkType = ProfilingSinkType::None;                        /** The type of profiling sink to use for this workflow execution. */
};

/** Optional WorkflowExecutionOptions for configuring workflow execution behavior */
using OptionalWorkflowExecutionOptions = std::optional<WorkflowExecutionOptions>;

}