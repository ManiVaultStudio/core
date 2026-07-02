// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

#include "WorkflowResultBase.h"

#include <QFuture>

namespace mv::workflow
{

/**
 * @brief Stores the outcome of a workflow stage execution.
 *
 * WorkflowStageResult specializes WorkflowResultBase for stage-level execution
 * results. It uses the common result status and optional QVariant output value
 * provided by the base class without adding extra stage-specific state.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class CORE_EXPORT WorkflowStageResult final : public WorkflowResultBase
{
public:

    /**
     * @brief Constructs a workflow stage result.
     * @param workflowName Name of the workflow that produced this stage result.
     */
    WorkflowStageResult(const QString& workflowName);
};

/** Unique ownership pointer type for workflow stage results. */
using UniqueWorkflowStageResult = std::unique_ptr<WorkflowStageResult>;

/** Shared ownership pointer type for workflow stage results. */
using SharedWorkflowStageResult = std::shared_ptr<WorkflowStageResult>;

/** Qt future type for asynchronous workflow stage results. */
using WorkflowStageFuture       = QFuture<WorkflowStageResult>;

}
