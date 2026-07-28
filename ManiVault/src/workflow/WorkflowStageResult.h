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
 * @brief Result of a workflow stage execution.
 *
 * WorkflowStageResult stores the status and optional value produced by a
 * workflow stage.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class CORE_EXPORT WorkflowStageResult final : public WorkflowResultBase
{
public:

    /**
     * @brief Constructs a workflow stage result.
     * @param workflowName Name of the workflow stage that produced this result.
     */
    WorkflowStageResult(const QString& workflowName);
};

/** Unique ownership pointer type for workflow stage results. */
using UniqueWorkflowStageResult = std::unique_ptr<WorkflowStageResult>;

/** Shared ownership pointer type for workflow stage results. */
using SharedWorkflowStageResult = std::shared_ptr<WorkflowStageResult>;

/** Qt future type for workflow stage results. */
using WorkflowStageFuture       = QFuture<WorkflowStageResult>;

}
