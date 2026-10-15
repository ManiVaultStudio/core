// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowExecutionContext.h"

#include "util/SeverityLevel.h"
#include <QUuid>
#include <QMutex>
#include <QMutexLocker>
#include <QTreeView>


namespace mv::workflow
{

/**
 * @brief Reports workflow context lifecycle events.
 *
 * WorkflowExecutionLifecycleScope reports that the supplied execution context
 * has started when the scope is constructed, measures elapsed execution time,
 * and reports completion when the scope is explicitly finished or destroyed.
 *
 * If fail() is called, the scope is marked as finished so the destructor does
 * not also report successful completion.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class CORE_EXPORT WorkflowExecutionLifecycleScope
{
public:

    /**
     * @brief Creates a lifecycle scope.
     * @param context Workflow execution context whose lifecycle is reported.
     */
    explicit WorkflowExecutionLifecycleScope(SharedWorkflowExecutionContext context);

    /**
     * @brief Destroys the lifecycle scope.
     *
     * If the scope has not already been completed, the destructor reports the
     * context as successfully finished.
     */
    ~WorkflowExecutionLifecycleScope();

    /**
     * @brief Reports the context as failed and completes the lifecycle scope.
     * @param severity Failure severity.
     * @param message Human-readable failure message.
     * @param details Additional failure details.
     */
    void fail(util::SeverityLevel severity, const QString& message, QVariantMap details = {});

    /**
     * @brief Reports failure without adding a message.
     */
    void failWithoutMessage();

    /**
     * @brief Reports successful completion with an explicit duration.
     * @param durationMs Duration in milliseconds.
     */
    void finish(std::uint64_t durationMs);

    /**
     * @brief Reports successful completion.
     */
    void finish();

    /**
     * @brief Returns the elapsed scope duration.
     * @return Elapsed wall-clock time in milliseconds.
     */
    [[nodiscard]] std::uint64_t elapsedMS() const;

private:

    SharedWorkflowExecutionContext  _context;           /**< Context whose lifecycle is reported. */
    QElapsedTimer                   _timer;             /**< Timer used to measure elapsed execution time. */
    bool                            _finished = false;  /**< Whether the lifecycle has already been completed. */
};

}
