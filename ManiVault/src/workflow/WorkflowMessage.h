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
 * @brief Message generated during workflow execution.
 *
 * Workflow messages are stored in report nodes and may also be formatted for
 * console output or result dialogs.
 *
 * @maintainer BioVault Thomas Kroes (Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
struct CORE_EXPORT WorkflowMessage
{
	util::SeverityLevel level = util::SeverityLevel::Info;          /**< Message severity */
    QString             emitter;                                    /**< Component or workflow entity that emitted the message */
    QString             location;                                   /**< Optional source or workflow location */
    QString             text;                                       /**< Human-readable message text */
    QVariantMap         details;                                    /**< Structured message details */
    QDateTime           timestamp = QDateTime::currentDateTime();   /**< Time when the message was created */
    QString             parentId;                                   /**< Optional parent message ID for hierarchical message organization (mirrors the workflow execution context identifier) */
};

/** Shared immutable workflow message reference. */
using SharedWorkflowMessage = std::shared_ptr<const workflow::WorkflowMessage>;

/** Collection of workflow messages. */
using WorkflowMessages = QVector<WorkflowMessage>;

}
