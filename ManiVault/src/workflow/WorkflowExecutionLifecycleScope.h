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
 * @brief RAII helper for reporting the lifecycle of a workflow execution context.
 *
 * WorkflowExecutionLifecycleScope reports that the supplied execution context has started
 * when the scope is constructed, measures the elapsed execution time, and reports completion
 * when the scope is explicitly finished or when the scope is destroyed.
 *
 * This class is intended to be used at execution boundaries, such as workflow, stage, or job
 * execution. It keeps lifecycle reporting centralized and prevents duplicated start/finish
 * logging code throughout the executor.
 *
 * If fail() is called, a failure is reported and the scope is marked as finished, preventing
 * the destructor from also reporting a successful finish.
 *
 * Typical usage:
 *
 * @code
 * auto jobContext = stageContext->createJobChild(job.getName());
 * WorkflowExecutionLifecycleScope lifecycle(jobContext);
 *
 * try {
 *     executeJob(job, jobContext);
 *     lifecycle.finish();
 * }
 * catch (const std::exception& e) {
 *     lifecycle.fail(QString::fromUtf8(e.what()));
 *     throw;
 * }
 * catch (...) {
 *     lifecycle.fail("Unknown exception");
 *     throw;
 * }
 * @endcode
 */
class CORE_EXPORT WorkflowExecutionLifecycleScope
{
public:

    /**
     * @brief Creates a lifecycle scope for the given workflow execution context.
     *
     * If the context is valid, the elapsed-time timer is started and the context receives
     * a started lifecycle report.
     *
     * @param context The workflow execution context whose lifecycle should be reported.
     */
    explicit WorkflowExecutionLifecycleScope(SharedWorkflowExecutionContext context);

    /**
     * @brief Destroys the lifecycle scope.
     *
     * If the scope has not already been completed using finish() or fail(), this destructor
     * reports the context as successfully finished using the elapsed time measured since
     * construction.
     */
    ~WorkflowExecutionLifecycleScope();

    /**
     * @brief Reports the context as failed and completes the lifecycle scope.
     *
     * Calling this function prevents the destructor from reporting a successful finish.
     * This should normally be called from an exception handler before rethrowing the exception.
     *
     * @param severity Severity level of the failure to include in the lifecycle report.
     * @param message Human-readable failure message to include in the lifecycle report.
     * @param details Additional details to include in the lifecycle report.
     */
    void fail(util::SeverityLevel severity, const QString& message, QVariantMap details = {});

    /**
     * @brief Reports successful completion.
     *
     * The reported duration is the elapsed time measured since construction. Calling this
     * function prevents the destructor from reporting completion a second time.
     */
    void finish(std::uint64_t durationMs);

    void finish();

    /**
	 * @brief Returns the elapsed time since this lifecycle scope was created.
	 *
	 * The elapsed time is measured in milliseconds using the internal QElapsedTimer.
	 * This can be used to copy the same measured duration into a WorkflowResult
	 * before the lifecycle scope is finished or destroyed.
	 *
	 * @return Elapsed wall-clock time in milliseconds.
	 */
    std::uint64_t elapsedMS() const;

private:

    SharedWorkflowExecutionContext  _context;           /**< Context whose lifecycle is reported */
    QElapsedTimer                   _timer;             /**< Timer used to measure elapsed execution time */
    bool                            _finished = false;  /**< Whether the lifecycle has already been completed */
};

}
