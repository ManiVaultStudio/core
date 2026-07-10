// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

namespace mv::workflow
{

/** Options for configuring the profiling behavior of a workflow. */
struct CORE_EXPORT WorkflowProfilingOptions
{
    /** The type of profiling sink to use for this workflow execution. */
    enum class SinkType
    {
        None,               /** No profiling or tracing will be performed. */
        ChromeTracing       /** Profiling data will be collected and output in a format compatible with Chrome's tracing tools. */
    };

    SinkType   sinkType = SinkType::None;     /** The type of profiling sink to use for this workflow execution. */
};

}