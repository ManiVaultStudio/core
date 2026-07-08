// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

namespace mv::workflow
{

/**
 * @brief Summary statistics for the execution of a workflow stage.
 *
 * WorkflowStageSummary aggregates high-level execution counters for a stage,
 * including the number of jobs started, finished, failed, and skipped, plus the
 * total stage duration in milliseconds.
 *
 * Summaries are primarily used for stage-level reporting, console output,
 * diagnostics, telemetry, and structured logging after all jobs in a stage have
 * completed.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
struct CORE_EXPORT WorkflowStageSummary
{
    int jobsStarted = 0;             /**< Number of jobs that entered execution */
    int jobsFinished = 0;            /**< Number of jobs that completed successfully */
    int jobsFailed = 0;              /**< Number of jobs that failed during execution */
    int jobsSkipped = 0;             /**< Number of jobs that were skipped */
    std::uint64_t durationMs = 0;    /**< Stage execution duration in milliseconds */

    /**
     * @brief Converts the stage summary to a QVariantMap.
     *
     * @return QVariantMap containing the stage summary metrics.
     */
    [[nodiscard]] QVariantMap toVariantMap() const;
};

}
