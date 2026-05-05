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
    static bool hasContext();

    static void info(const QString& text, const QString& source = {}, const QString& scope = {}, QVariantMap details = {});

    static void warning(const QString& text, const QString& source = {}, const QString& scope = {}, QVariantMap details = {});

    static void error(const QString& text, const QString& source = {}, const QString& scope = {}, QVariantMap details = {});

    static void message(SeverityLevel severity, const QString& text, const QString& source = {}, const QString& code = {}, const QString& scope = {}, QVariantMap details = {});

    static void setProgress(double value);
};

} // namespace mv::util