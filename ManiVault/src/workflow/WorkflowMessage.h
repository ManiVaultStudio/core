// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

#include "util/SeverityLevel.h"

#include <QString>
#include <QDateTime>

namespace mv::workflow
{

/**
 * Represents a message generated during workflow execution, containing information about the severity, emitter, location, text, details, and timestamp of the message.
 *
 * @authors BioVault Thomas Kroes (Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
struct CORE_EXPORT WorkflowMessage
{
	util::SeverityLevel level = util::SeverityLevel::Info;          /** The severity level of the message (e.g., Info, Warning, Error, Fatal) */
    QString             emitter;                                    /** The name of the component or module that generated the message */
    QString             location;                                   /** The specific location in the code or workflow where the message was generated (e.g., function name, line number) */
    QString             text;                                       /** The main text or content of the message */
    QVariantMap         details;                                    /** Additional details or metadata associated with the message */
    QDateTime           timestamp = QDateTime::currentDateTime();   /** The timestamp when the message was generated */
};

using SharedWorkflowMessage = std::shared_ptr<const workflow::WorkflowMessage>;
using WorkflowMessages = QVector<WorkflowMessage>;

}