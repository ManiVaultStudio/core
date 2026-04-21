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
        , _context(&context)
    {
        WorkflowExecutionContext::setCurrent(_context);
    }

    ~WorkflowExecutionScope()
    {
        WorkflowExecutionContext::setCurrent(_previous);
    }

private:
    WorkflowExecutionContext* _previous = nullptr;
    WorkflowExecutionContext* _context = nullptr;
};

}
