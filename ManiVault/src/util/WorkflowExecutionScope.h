// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowExecutionContext.h"

namespace mv::util
{

class WorkflowExecutionScope
{
public:
    explicit WorkflowExecutionScope(WorkflowExecutionContext& ctx)
        : _previous(WorkflowExecutionContext::current())
    {
        WorkflowExecutionContext::setCurrent(&ctx);
    }

    explicit WorkflowExecutionScope(const WorkflowExecutionContext& ctx)
        : _previous(WorkflowExecutionContext::current())
        , _currentCopy(std::make_unique<WorkflowExecutionContext>(ctx))
    {
        WorkflowExecutionContext::setCurrent(_currentCopy.get());
    }

    ~WorkflowExecutionScope()
    {
        WorkflowExecutionContext::setCurrent(_previous);
    }

    WorkflowExecutionScope(const WorkflowExecutionScope&) = delete;
    WorkflowExecutionScope& operator=(const WorkflowExecutionScope&) = delete;

    WorkflowExecutionScope(WorkflowExecutionScope&&) = delete;
    WorkflowExecutionScope& operator=(WorkflowExecutionScope&&) = delete;

private:
    WorkflowExecutionContext* _previous = nullptr;
    std::unique_ptr<WorkflowExecutionContext> _currentCopy;
};

}
