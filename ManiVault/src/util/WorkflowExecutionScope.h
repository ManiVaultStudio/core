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
    explicit WorkflowExecutionScope(WorkflowExecutionContext& context)
        : _previous(WorkflowExecutionContext::current())
    {
        WorkflowExecutionContext::setCurrent(&context);
    }

    explicit WorkflowExecutionScope(const WorkflowExecutionContext& context)
        : _previous(WorkflowExecutionContext::current())
        , _ownedContext(std::make_unique<WorkflowExecutionContext>(context))
    {
        WorkflowExecutionContext::setCurrent(_ownedContext.get());
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
    std::unique_ptr<WorkflowExecutionContext> _ownedContext;
};

}
