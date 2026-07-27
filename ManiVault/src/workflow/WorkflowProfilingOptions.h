// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

namespace mv::workflow
{

/**
 * @brief Configures workflow profiling and tracing.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
struct CORE_EXPORT WorkflowProfilingOptions
{
    /**
     * @brief Defines the profiling output sink.
     */
    enum class SinkType
    {
        None,               /**< No profiling or tracing. */
        ChromeTracing       /**< Chrome tracing-compatible output. */
    };

    SinkType   sinkType = SinkType::None;     /**< Profiling sink used for execution. */
};

}
