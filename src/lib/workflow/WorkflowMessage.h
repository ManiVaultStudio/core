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
 * @brief Defines the workflow message category.
 *
 * Lifecycle messages describe execution state transitions. Diagnostic messages
 * describe warnings, errors, or informational details emitted during execution.
 */
enum class MessageKind
{
    Lifecycle,      /**< Message related to the lifecycle of a workflow entity. */
    Diagnostic      /**< Message providing diagnostic information about workflow execution. */
};

/**
 * @brief Message generated during workflow execution.
 *
 * Workflow messages are stored in report nodes and may also be formatted for
 * console output or result dialogs.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
struct CORE_EXPORT WorkflowMessage
{
    util::SeverityLevel level = util::SeverityLevel::Info;          /**< Message severity. */
    QString             emitter;                                    /**< Component or workflow entity that emitted the message. */
    QString             location;                                   /**< Optional source or workflow location. */
    QString             text;                                       /**< Human-readable message text. */
    QVariantMap         details;                                    /**< Structured message details. */
    QDateTime           timestamp = QDateTime::currentDateTime();   /**< Time when the message was created. */
    QString             contextId;                                  /**< Identifier of the workflow execution context that emitted the message. */
    QString             parentContextId;                            /**< Identifier of the parent workflow execution context, if any. */
};

/** Shared immutable workflow message reference. */
using SharedWorkflowMessage = std::shared_ptr<const workflow::WorkflowMessage>;

/** Collection of workflow messages. */
using WorkflowMessages = QVector<WorkflowMessage>;

}
