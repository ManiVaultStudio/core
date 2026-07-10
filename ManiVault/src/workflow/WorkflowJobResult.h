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
 * @brief Result of a workflow job execution.
 *
 * WorkflowJobResult stores the status and optional value produced by a single
 * workflow job.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class CORE_EXPORT WorkflowJobResult final : public WorkflowResultBase
{
public:

    /**
     * @brief Constructs a workflow job result.
     * @param workflowName The name of the workflow that produced this result. This can be used for identification and logging purposes, allowing you to associate the result with a specific workflow execution.
     */
    WorkflowJobResult(const QString& workflowName);
};

using UniqueWorkflowJobResult   = std::unique_ptr<WorkflowJobResult>;
using SharedWorkflowJobResult   = std::shared_ptr<WorkflowJobResult>;
using WorkflowJobResultFuture   = QFuture<WorkflowJobResult>;

}
