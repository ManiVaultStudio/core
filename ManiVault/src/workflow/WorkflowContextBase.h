// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

#include <memory>

namespace mv::workflow
{

/**
 * @brief Optional workflow-specific context shared by all stages and jobs.
 *
 * The context is the intended mechanism for sharing input, output, and
 * intermediate data across a workflow. Implementations should use a typed
 * context class when workflow stages need to communicate.
 *
 * Thread safety is the responsibility of the context implementation. For
 * parallel stages, prefer per-job output storage followed by a sequential
 * merge stage, or protect shared mutable state with appropriate locking.
 *
 * @author Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class CORE_EXPORT WorkflowContextBase
{
public:
    virtual ~WorkflowContextBase() = default;
};

using UniqueWorkflowContext = std::unique_ptr<WorkflowContextBase>;
using SharedWorkflowContext = std::shared_ptr<WorkflowContextBase>;

}
