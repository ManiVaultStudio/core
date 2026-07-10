// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

namespace mv::workflow
{

struct CORE_EXPORT WorkflowReportingOptions
{
    bool            reportProgress = false;                                         /** Whether to report progress during workflow execution. (with a modal task) */
    bool            addNotification = false;                                        /** Whether to add notifications during workflow execution. */
    bool            enableConsoleDashboard = false;                                 /** Whether to enable the console dashboard for this workflow execution. This will be ignored if a trace sink is provided, as the trace sink is responsible for managing its own dashboard if needed. */
    std::uint32_t   maxLoggingDepth = std::numeric_limits<std::uint32_t>::max();    /** The maximum depth of logging for this workflow execution. This controls how many levels of nested operations will be logged. Adjust this value based on the expected complexity of the workflow and the desired level of detail in the logs. */
};

}