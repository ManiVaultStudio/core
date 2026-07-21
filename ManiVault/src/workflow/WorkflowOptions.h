// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowParallelizationOptions.h"
#include "WorkflowBatchingOptions.h"
#include "WorkflowReportingOptions.h"
#include "WorkflowProfilingOptions.h"

#include <optional>

namespace mv::workflow
{

/** Options that configure workflow execution behavior. */
struct CORE_EXPORT WorkflowOptions
{
    WorkflowParallelizationOptions  execution;      /**< Parallelization options for workflow jobs */
	WorkflowBatchingOptions         batching;       /**< Batch-size options for serialization work */
    WorkflowReportingOptions        reporting;      /**< Reporting, notification, and result-dialog options */
    WorkflowProfilingOptions        profiling;      /**< Profiling and trace sink options */
};

/** Optional workflow execution options. */
using OptionalWorkflowOptions = std::optional<WorkflowOptions>;

}
