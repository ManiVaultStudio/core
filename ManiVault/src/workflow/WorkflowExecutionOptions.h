// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowBatchingOptions.h"
#include "WorkflowReportingOptions.h"

namespace mv::workflow
{

using MaxWorkerThreadCount = std::uint32_t;

/** Options for configuring the execution behavior of a workflow. These options control aspects such as parallel execution, batching behavior, and profiling. Adjust these options based on the expected workload and system capabilities to achieve optimal performance without overwhelming system resources. */
struct CORE_EXPORT WorkflowExecutionOptions
{
    enum class ProfilingSinkType
    {
        None,               /** No profiling or tracing will be performed. */
        ChromeTracing       /** Profiling data will be collected and output in a format compatible with Chrome's tracing tools. */
    };

    bool                        parallel = true;                                                    /** Whether to execute the workflow in parallel using multiple threads (if supported by the workflow plan) or sequentially on a single thread. */
    MaxWorkerThreadCount        maxWorkerThreadCount = 63;                                          /** The maximum number of worker threads to use for parallel execution. This is ignored if parallel is false. The default value of 63 allows for up to 64 threads total (including the main thread) on platforms with a maximum of 64 concurrent threads. Adjust this value based on the expected workload and system capabilities. */
    WorkflowBatchingOptions     workflowBatchingOptions;                                            /** Batching options for workflow execution. These options control the degree of parallelism for various operations during workflow execution, such as loading and saving datasets, and encoding and decoding data blocks. Adjust these options based on the expected workload and system capabilities to achieve optimal performance without overwhelming system resources. (under normal circumstances this should be sufficient) */
    WorkflowReportingOptions    reportingOptions;                                                    /** Reporting options for workflow execution. These options control whether progress is reported, notifications are added, and the console dashboard is enabled during workflow execution. Adjust these options based on the desired level of feedback and monitoring during workflow execution. */
    ProfilingSinkType           profilingSinkType = ProfilingSinkType::None;                        /** The type of profiling sink to use for this workflow execution. */
};

/** Optional WorkflowExecutionOptions for configuring workflow execution behavior */
using OptionalWorkflowExecutionOptions = std::optional<WorkflowExecutionOptions>;

}