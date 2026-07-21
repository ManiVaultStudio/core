// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowConsoleDashboard.h"

namespace mv::workflow
{

/**
 * @brief RAII scope that starts and stops a workflow console dashboard.
 */
class CORE_EXPORT WorkflowConsoleDashboardScope
{
public:

    /**
     * @brief Starts a console dashboard for the supplied execution state.
     * @param state Execution state observed by the dashboard.
     */
    explicit WorkflowConsoleDashboardScope(WorkflowExecutionState::Ptr state);

    /** Stops the dashboard. */
    ~WorkflowConsoleDashboardScope();

private:

    std::unique_ptr<WorkflowConsoleDashboard> _dashboard;  /**< Owned console dashboard */
};

}
