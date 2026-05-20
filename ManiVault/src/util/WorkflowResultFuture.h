// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "WorkflowResult.h"

#include <QFuture>
#include <QPointer>
#include <QFutureWatcher>

#include <exception>

namespace mv
{
    class Task;
}

namespace mv::util
{

class CORE_EXPORT WorkflowResultFuture
{
public:
    struct CORE_EXPORT State {
        QFuture<SharedWorkflowResult> future;
        QPointer<QFutureWatcher<SharedWorkflowResult>> watcher;

        QPointer<Task> task;

        mutable QMutex mutex;
        std::exception_ptr exception;

        void setException(std::exception_ptr e);

        std::exception_ptr getException() const;

        bool hasException() const;

        void rethrowExceptionIfAny() const;
    };

public:
    WorkflowResultFuture();

    explicit WorkflowResultFuture(std::shared_ptr<State> state);

    bool isValid() const;

    bool isFinished() const;

    void waitForFinished() const;

    SharedWorkflowResult result() const;

    const QFuture<SharedWorkflowResult>& getFuture() const;

    Task* getTask() const;

    QFutureWatcher<SharedWorkflowResult>* getWatcher() const;
    static WorkflowResultFuture makeReady(const SharedWorkflowResult& result = {});

    static WorkflowResultFuture fromFuture(QFuture<SharedWorkflowResult> future);

    std::shared_ptr<State> getState() const { return _state; }
private:
    std::shared_ptr<State> _state;
};

using WorkflowResultFutures = std::vector<WorkflowResultFuture>;

}