// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

namespace mv::util
{

class CORE_EXPORT WorkflowConsoleFormatter
{
public:
    static QString format(SeverityLevel severity, const QString& text, const QString& location, const QVariantMap& details);

private:
    static QString iconForEvent(const QString& event);
    static QString labelForEntity(const QString& entity);
};

}
