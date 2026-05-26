// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

namespace mv::util
{

    /**
 * @brief Summary statistics for the execution of a workflow stage.
 *
 * WorkflowStageSummary aggregates high-level execution metrics for a stage, such as the
 * number of jobs that started, completed successfully, failed, or were skipped, along
 * with the total execution duration of the stage.
 *
 * This structure is primarily intended for:
 * - Stage-level workflow reporting
 * - Console and UI summaries
 * - Workflow diagnostics and telemetry
 * - Execution statistics aggregation
 * - Structured logging and analytics
 *
 * A stage summary is typically generated after all jobs belonging to a stage have completed.
 *
 * Example console output:
 *
 * @code
 * [OK] Parallel stage: Load datasets
 *       jobs: 16
 *       completed: 15
 *       failed: 1
 *       skipped: 0
 *       duration: 4213 ms
 * @endcode
 */
struct CORE_EXPORT WorkflowStageSummary
{
    /**
     * @brief Total number of jobs that were started for the stage.
     *
     * This includes all jobs that entered execution, regardless of whether they
     * eventually completed successfully, failed, or were cancelled.
     */
    int jobsStarted = 0;

    /**
     * @brief Total number of jobs that completed successfully.
     *
     * A job is considered finished when it completed execution without throwing
     * an exception or reporting a fatal workflow error.
     */
    int jobsFinished = 0;

    /**
     * @brief Total number of jobs that failed during execution.
     *
     * A failed job is a job that terminated due to an exception or a workflow
     * error with severity Error or Fatal.
     */
    int jobsFailed = 0;

    /**
     * @brief Total number of jobs that were skipped.
     *
     * Jobs may be skipped for various reasons, such as:
     * - Disabled stages
     * - Conditional execution logic
     * - Empty parallel stages
     * - Dependency failures
     */
    int jobsSkipped = 0;

    /**
     * @brief Total execution duration of the stage in milliseconds.
     *
     * The duration typically represents the elapsed wall-clock time between
     * stage start and stage completion.
     */
    std::uint64_t durationMs = 0;

    /**
     * @brief Converts the stage summary to a QVariantMap.
     *
     * This is useful for:
     * - Structured logging
     * - Serialization
     * - Workflow reporting
     * - Diagnostics and telemetry systems
     * - UI integration
     *
     * @return QVariantMap containing the stage summary metrics.
     */
    QVariantMap toVariantMap() const;
};

}
