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
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
struct CORE_EXPORT WorkflowMessage
{
    util::SeverityLevel level = util::SeverityLevel::Info;          /**< Message severity */
    QString             emitter;                                    /**< Component, workflow, or subsystem that emitted the message */
    QString             location;                                   /**< Source or workflow location associated with the message */
    QString             text;                                       /**< Human-readable message text */
    QVariantMap         details;                                    /**< Optional structured message metadata */
    QDateTime           timestamp = QDateTime::currentDateTime();   /**< Timestamp when the message was created */
};

/** Shared pointer type for immutable workflow messages. */
using SharedWorkflowMessage = std::shared_ptr<const workflow::WorkflowMessage>;

/** Ordered collection of workflow messages. */
using WorkflowMessages = QVector<WorkflowMessage>;

}
