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
        QFuture<SharedWorkflowResult> future;
        QPointer<Task> task;
        QPointer<QFutureWatcher<SharedWorkflowResult>> watcher;
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

private:
    std::shared_ptr<State> _state;
};

} // namespace mv::util