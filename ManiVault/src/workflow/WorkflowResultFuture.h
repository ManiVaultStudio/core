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
 * @brief Future for an asynchronously executing workflow.
 */
class CORE_EXPORT WorkflowResultFuture
{
public:

    /** Shared asynchronous workflow execution state. */
    struct CORE_EXPORT State {
        std::future<SharedWorkflowResult> future;   /**< Future that produces the workflow result */
        QPointer<Task> task;                         /**< Optional task associated with execution */

        mutable QMutex mutex;                        /**< Protects stored exception state */
        std::exception_ptr exception;                /**< Exception captured from asynchronous execution */

        /** Stores an asynchronous execution exception. */
        void setException(std::exception_ptr e);

        /** @return Stored asynchronous execution exception. */
        std::exception_ptr getException() const;

        /** @return True when an exception has been stored. */
        bool hasException() const;

        /** Rethrows the stored exception, if any. */
        void rethrowExceptionIfAny() const;
    };

public:

    /** Constructs an empty workflow result future. */
    WorkflowResultFuture();

    /** Constructs a workflow result future from shared state. */
    explicit WorkflowResultFuture(std::shared_ptr<State> state);

    /** @return Completed workflow result, blocking until execution finishes. */
    [[nodiscard]] SharedWorkflowResult get() const;

    /** @return Task associated with the asynchronous workflow, or nullptr. */
    Task* getTask() const;

    /** @return Shared asynchronous execution state. */
    std::shared_ptr<State> getState() const { return _state; }

    /** Registers a callback to run when the workflow finishes. */
    void onFinished(QObject* receiver, std::function<void(SharedWorkflowResult)> callback);

private:

    std::shared_ptr<State> _state;   /**< Shared asynchronous execution state */
};

/** Collection of workflow result futures. */
using WorkflowResultFutures = std::vector<WorkflowResultFuture>;

}
