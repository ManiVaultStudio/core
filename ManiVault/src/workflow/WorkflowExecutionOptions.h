// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowBatchingOptions.h"

namespace mv::workflow
{

using MaxWorkerThreadCount = std::uint32_t;

/** Options controlling workflow execution behavior. */
struct CORE_EXPORT WorkflowExecutionOptions
{
    /** Profiling output sink used during workflow execution. */
    enum class ProfilingSinkType
    {
        None,           /**< Profiling and tracing are disabled */
        ChromeTracing   /**< Profiling data is emitted in Chrome tracing format */
    };

    bool                    parallel = true;                                                /**< Enables parallel execution when supported by the workflow plan */
    MaxWorkerThreadCount    maxWorkerThreadCount = 63;                                      /**< Maximum number of worker threads used for parallel execution */
    bool                    reportProgress = false;                                         /**< Enables modal task progress reporting */
    bool                    addNotification = false;                                        /**< Enables workflow completion notifications */
    bool                    enableConsoleDashboard = false;                                 /**< Enables the console dashboard during workflow execution */
    std::uint32_t           maxLoggingDepth = std::numeric_limits<std::uint32_t>::max();    /**< Maximum nested detail depth for workflow logging */
    WorkflowBatchingOptions workflowBatchingOptions;                                        /**< Batching options used by workflow execution */
    ProfilingSinkType       profilingSinkType = ProfilingSinkType::None;                    /**< Profiling sink used by this workflow execution */
};

/** Optional workflow execution options. */
using OptionalWorkflowExecutionOptions = std::optional<WorkflowExecutionOptions>;

}
