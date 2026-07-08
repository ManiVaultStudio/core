// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "WorkflowResult.h"

#include <QPointer>
#include <QMutex>

#include <exception>
#include <future>

namespace mv
{
    class Task;
}

namespace mv::workflow
{

/**
 * @brief Handle for asynchronously produced workflow results.
 *
 * WorkflowResultFuture owns shared state for a background workflow execution.
 * It exposes blocking result retrieval through get(), optional access to the
 * associated Task, and a Qt-friendly completion callback that is delivered to a
 * receiver object using a queued connection.
 *
 * Exceptions raised while waiting for the result are captured in the shared
 * state and rethrown when the result is retrieved or when the completion
 * callback is delivered.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class CORE_EXPORT WorkflowResultFuture
{
public:

    /**
     * @brief Shared asynchronous workflow result state.
     *
     * State stores the underlying std::future, the optional Task associated
     * with the workflow execution, and any exception captured while waiting for
     * completion from a helper thread.
     */
    struct CORE_EXPORT State {
        std::future<SharedWorkflowResult>  future;     /**< Future that produces the shared workflow result */
        QPointer<Task>                     task;       /**< Optional task associated with the workflow execution */

        mutable QMutex                     mutex;      /**< Protects access to the captured exception */
        std::exception_ptr                 exception;  /**< Exception captured while waiting for workflow completion */

        /**
         * @brief Stores an exception captured during asynchronous completion.
         * @param e Exception pointer to store.
         */
        void setException(std::exception_ptr e);

        /**
         * @brief Returns the captured exception, if any.
         * @return Captured exception pointer.
         */
        [[nodiscard]] std::exception_ptr getException() const;

        /**
         * @brief Returns whether an exception has been captured.
         * @return True if a captured exception exists.
         */
        [[nodiscard]] bool hasException() const;

        /**
         * @brief Rethrows the captured exception if one exists.
         */
        void rethrowExceptionIfAny() const;
    };

public:

    /**
     * @brief Constructs an invalid workflow result future.
     */
    WorkflowResultFuture();

    /**
     * @brief Constructs a workflow result future from shared state.
     * @param state Shared asynchronous result state.
     */
    explicit WorkflowResultFuture(std::shared_ptr<State> state);

    /**
     * @brief Blocks until the workflow result is available.
     *
     * Any exception captured in the shared state is rethrown after the future
     * has completed.
     *
     * @return Shared workflow result.
     */
    [[nodiscard]] SharedWorkflowResult get() const;

    /**
     * @brief Returns the task associated with this workflow execution.
     * @return Associated task, or nullptr if none is available.
     */
    [[nodiscard]] Task* getTask() const;

    /**
     * @brief Returns the shared asynchronous result state.
     * @return Shared state object.
     */
    [[nodiscard]] std::shared_ptr<State> getState() const { return _state; }

    /**
     * @brief Invokes a callback when the workflow result becomes available.
     *
     * Waiting occurs on a detached helper thread. The callback is delivered to
     * the receiver's thread through QMetaObject::invokeMethod() using a queued
     * connection. If the receiver is destroyed before completion, the callback
     * is skipped.
     *
     * @param receiver QObject that receives the queued callback.
     * @param callback Function invoked with the completed workflow result.
     */
    void onFinished(QObject* receiver, std::function<void(SharedWorkflowResult)> callback);

private:
    std::shared_ptr<State> _state;                     /**< Shared asynchronous result state */
};

/** Collection of asynchronous workflow result handles. */
using WorkflowResultFutures = std::vector<WorkflowResultFuture>;

}
