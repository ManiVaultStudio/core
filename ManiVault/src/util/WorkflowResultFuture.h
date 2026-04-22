// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "WorkflowResult.h"

#include <QFuture>
#include <QPointer>
#include <QFutureWatcher>

namespace mv::util
{

class CORE_EXPORT WorkflowResultFuture
{
public:
    struct State
    {
        QFuture<WorkflowResult> future;
        QPointer<Task> task;
        QPointer<QFutureWatcher<WorkflowResult>> watcher;
    };

public:
    WorkflowResultFuture() = default;

    explicit WorkflowResultFuture(std::shared_ptr<State> state)
        : _state(std::move(state))
    {
    }

    bool isValid() const
    {
        return _state && _state->future.isValid();
    }

    bool isFinished() const
    {
        return _state && _state->future.isFinished();
    }

    void waitForFinished() const
    {
        if (_state)
            _state->future.waitForFinished();
    }

    WorkflowResult result() const
    {
        if (!_state)
            return {};

        return _state->future.result();
    }

    const QFuture<WorkflowResult>& getFuture() const
    {
        Q_ASSERT(_state);
        return _state->future;
    }

    Task* getTask() const
    {
        return _state ? _state->task.data() : nullptr;
    }

    QFutureWatcher<WorkflowResult>* getWatcher() const
    {
        return _state ? _state->watcher.data() : nullptr;
    }

    static WorkflowResultFuture makeReady(const WorkflowResult& result = {})
    {
        auto state = std::make_shared<State>();

        QPromise<WorkflowResult> promise;
        state->future = promise.future();
        promise.start();
        promise.addResult(result);
        promise.finish();

        return WorkflowResultFuture(state);
    }

    static WorkflowResultFuture fromFuture(QFuture<WorkflowResult> future)
    {
        auto state = std::make_shared<State>();
        state->future = std::move(future);
        return WorkflowResultFuture(state);
    }

private:
    std::shared_ptr<State> _state;
};

} // namespace mv::util