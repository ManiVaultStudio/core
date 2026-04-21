// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "WorkflowResult.h"

#include <QFuture>

namespace mv::util
{

class CORE_EXPORT WorkflowResultFuture
{
public:
    WorkflowResultFuture() = default;

    WorkflowResultFuture(QFuture<WorkflowResult> future, Task* task = nullptr)
        : _future(std::move(future))
        , _task(task)
    {
    }

    bool isValid() const
    {
        return _future.isValid();
    }

    bool isFinished() const
    {
        return _future.isFinished();
    }

    void waitForFinished()
    {
        _future.waitForFinished();
    }

    WorkflowResult result() const
    {
        return _future.result();
    }

    const QFuture<WorkflowResult>& getFuture() const
    {
        return _future;
    }

    Task* getTask() const
    {
        return _task;
    }

private:
    QFuture<WorkflowResult> _future;
    Task* _task = nullptr;
};

} // namespace mv::util