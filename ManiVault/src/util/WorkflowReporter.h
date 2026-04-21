// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowExecutionContext.h"

#include <QString>

namespace mv::util
{

class CORE_EXPORT WorkflowReporter
{
public:
    static bool hasContext()
    {
        return WorkflowExecutionContext::current() != nullptr;
    }

    static void info(const QString& text, const QString& source = {})
    {
        if (auto* context = WorkflowExecutionContext::current())
            context->info(text, source);
    }

    static void warning(const QString& text, const QString& source = {})
    {
        if (auto* context = WorkflowExecutionContext::current())
            context->warning(text, source);
    }

    static void error(const QString& text, const QString& source = {})
    {
        if (auto* context = WorkflowExecutionContext::current())
            context->error(text, source);
    }

    static void setProgress(double value)
    {
        if (auto* context = WorkflowExecutionContext::current())
            context->setProgress(value);
    }
};

} // namespace mv::util