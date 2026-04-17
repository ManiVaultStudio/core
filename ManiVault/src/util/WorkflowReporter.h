// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowExecutionContext.h"

#include <QString>

namespace mv::util
{

class WorkflowReporter
{
public:
    static void info(const QString& text, const QString& source = {})
    {
        if (auto* ctx = WorkflowExecutionContext::current())
            ctx->info(text, source);
    }

    static void warning(const QString& text, const QString& source = {})
    {
        if (auto* ctx = WorkflowExecutionContext::current())
            ctx->warning(text, source);
    }

    static void error(const QString& text, const QString& source = {})
    {
        if (auto* ctx = WorkflowExecutionContext::current())
            ctx->error(text, source);
    }

    static void setProgress(double value)
    {
        if (auto* ctx = WorkflowExecutionContext::current())
            ctx->setProgress(value);
    }

    static bool hasContext()
    {
        return WorkflowExecutionContext::current() != nullptr;
    }
};

} // namespace mv::util