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
 * @brief Represents a message generated during workflow execution.
 *
 * WorkflowMessage is the fundamental diagnostic record used throughout the
 * workflow system. It captures informational messages, warnings, errors, and
 * fatal failures together with optional structured metadata describing the
 * execution context in which the message was generated.
 *
 * Workflow messages are attached to WorkflowReportNode instances and are used
 * for logging, reporting, diagnostics, and user-facing workflow output.
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