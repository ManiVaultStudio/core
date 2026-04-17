// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

#include <QString>
#include <QDateTime>

namespace mv::util
{

/**
 * Workflow message level
 *
 * Enumeration for the level of a workflow message, which can be either informational, a warning or an error.
 */
enum class WorkflowMessageLevel {
    Info,
    Warning,
    Error
};

/**
 * Workflow message
 *
 * Class for representing a workflow message, which contains a level, source, text and timestamp.
 */
struct CORE_EXPORT WorkflowMessage
{
    WorkflowMessageLevel _level = WorkflowMessageLevel::Info;           /** Level of the message (info, warning or error) */
    QString              _source;                                       /** Source of the message */
    QString              _text;                                         /** Text of the message */
    QDateTime            _timestamp = QDateTime::currentDateTime();     /** Timestamp of the message */
};

} // namespace mv::util