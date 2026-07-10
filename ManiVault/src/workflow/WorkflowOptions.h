// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowParallelizationOptions.h"
#include "WorkflowBatchingOptions.h"
#include "WorkflowReportingOptions.h"
#include "WorkflowProfilingOptions.h"

namespace mv::workflow
{

/** Options for configuring the execution behavior of a workflow. */
struct CORE_EXPORT WorkflowOptions
{
    WorkflowParallelizationOptions  execution;      /** Parallelization options for workflow execution. These options control whether parallel execution is enabled, and if so, the maximum number of worker threads to use for executing workflow jobs. */
	WorkflowBatchingOptions         batching;       /** Batching options for workflow execution. These options control the degree of parallelism for various operations during workflow execution, such as loading and saving datasets, and encoding and decoding data blocks. */
    WorkflowReportingOptions        reporting;      /** Reporting options for workflow execution. These options control whether progress is reported, notifications are added, and the console dashboard is enabled during workflow execution. */
    WorkflowProfilingOptions        profiling;      /** Profiling options for workflow execution. These options control whether profiling is enabled, and if so, the type of profiling sink to use for collecting and outputting profiling data.  */
};

/** Optional WorkflowOptions for configuring workflow execution behavior */
using OptionalWorkflowOptions = std::optional<WorkflowOptions>;

}