// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowResultDetailsOptions.h"

#include <QString>

#include <cstdint>
#include <limits>

namespace mv::workflow
{

/**
 * @brief Configures workflow progress and reporting behavior.
 *
 * These options control whether workflow execution creates visible progress,
 * emits completion notifications, produces live console output, and customizes
 * the result dialog copy.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
struct CORE_EXPORT WorkflowReportingOptions
{
    bool                            progress = false;                                               /**< Whether to show workflow progress through a task. */
    bool                            finishedNotification = false;                                   /**< Whether to add a notification when execution finishes. */
    bool                            enableConsoleDashboard = false;                                 /**< Whether to enable live console dashboard output. */
    std::uint32_t                   maxLoggingDepth = std::numeric_limits<std::uint32_t>::max();    /**< Maximum nested detail depth for workflow logging. */
    WorkflowResultDetailsOptions    resultDetails;                                                  /**< Result dialog title and message options. */
};

}
