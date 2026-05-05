// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "SeverityLevel.h"

#include <QString>
#include <QDateTime>

namespace mv::util
{

struct CORE_EXPORT WorkflowMessage
{
    SeverityLevel   _level = SeverityLevel::Info;
    QString         _source;
    QString         _text;
    QVariantMap     _details;
    QString         _nodeName;
    QDateTime       _timestamp = QDateTime::currentDateTime();
};

using WorkflowMessages = QVector<WorkflowMessage>;

} // namespace mv::util