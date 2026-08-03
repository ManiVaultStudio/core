// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "WorkflowResult.h"

#include <QObject>
#include <QPointer>
#include <QMutex>

#include <exception>
#include <functional>
#include <future>
#include <memory>
#include <vector>

namespace mv
{
    class Task;
}

namespace mv::workflow
{

/**
 * @brief Represents an asynchronous workflow result.
 *
 * WorkflowResultFuture wraps the shared future returned by asynchronous
 * workflow execution together with its optional GUI task and captured
 * exception state.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class CORE_EXPORT WorkflowResultFuture
{
public:

    /**
     * @brief Shared state for an asynchronous workflow execution.
     */
    struct CORE_EXPORT State
    {
        std::shared_future<SharedWorkflowResult>    future;         /**< Future that produces the workflow result. */
        QPointer<Task>                              task;           /**< Optional task associated with execution. */
        mutable QMutex                              mutex;          /**< Protects stored exception state. */
        std::exception_ptr                          exception;      /**< Exception captured from asynchronous execution. */

        /**
         * @brief Stores an asynchronous execution exception.
         * @param e Exception pointer to store.
         */
        void setException(std::exception_ptr e);

        /**
         * @brief Returns the stored asynchronous execution exception.
         * @return Stored exception pointer.
         */
        [[nodiscard]] std::exception_ptr getException() const;

        /**
         * @brief Returns whether an exception has been stored.
         * @return True if an exception is stored.
         */
        [[nodiscard]] bool hasException() const;

        /**
         * @brief Rethrows the stored exception.
         *
         * If no exception has been stored, the function returns without doing
         * anything.
         */
        void rethrowExceptionIfAny() const;
    };

public:

    /**
     * @brief Constructs an empty workflow result future.
     */
    WorkflowResultFuture();

    /**
     * @brief Constructs a workflow result future from shared state.
     * @param state Shared asynchronous execution state.
     */
    explicit WorkflowResultFuture(std::shared_ptr<State> state);

    /**
     * @brief Returns the completed workflow result.
     *
     * This call blocks until the asynchronous execution has finished and
     * rethrows any exception captured by the shared state.
     *
     * @return Completed workflow result.
     */
    [[nodiscard]] SharedWorkflowResult get() const;

    /**
     * @brief Returns the associated GUI task.
     * @return Task associated with execution, or nullptr.
     */
    [[nodiscard]] Task* getTask() const;

    /**
     * @brief Returns the shared asynchronous execution state.
     * @return Shared state object.
     */
    [[nodiscard]] std::shared_ptr<State> getState() const { return _state; }

    /**
     * @brief Registers a callback for workflow completion.
     * @param receiver QObject that owns the callback connection lifetime.
     * @param callback Function invoked with the completed workflow result.
     */
    void onFinished(QObject* receiver, std::function<void(SharedWorkflowResult)> callback);

private:

    std::shared_ptr<State> _state;   /**< Shared asynchronous execution state. */
};

/** Collection of workflow result futures. */
using WorkflowResultFutures = std::vector<WorkflowResultFuture>;

}
