// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowConsoleDashboard.h"

namespace mv::workflow
{

/**
 * @brief RAII scope for managing a WorkflowConsoleDashboard instance.
 *
 * This class creates a WorkflowConsoleDashboard for a given WorkflowExecutionState
 * and ensures that it is properly destroyed when the scope ends. It is intended to
 * be used in contexts where a console dashboard is needed for monitoring workflow
 * execution progress, and it automatically handles the lifecycle of the dashboard.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class CORE_EXPORT WorkflowConsoleDashboardScope
{
public:

    /**
     * @brief Constructs a workflow console dashboard scope.
     *
     * @param state Shared workflow execution state to observe.
     */
    explicit WorkflowConsoleDashboardScope(WorkflowExecutionState::Ptr state);

    /**
     * @brief Destroys the scope and stops the associated dashboard.
     *
     * This destructor ensures that the dashboard is properly stopped and cleaned up
     * when the scope ends. It is safe to use this class in a RAII manner to manage
     * the lifecycle of the console dashboard.
     */
    ~WorkflowConsoleDashboardScope();

private:
    std::unique_ptr<WorkflowConsoleDashboard>       _dashboard;     /**< Console dashboard managed by this scope */
};

}
