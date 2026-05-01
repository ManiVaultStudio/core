// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowMessage.h"

namespace mv::util
{
QString getWorkflowMessageLevelName(WorkflowMessageLevel level)
{
    if (const auto it = workflowMessageLevelNames.find(level); it != workflowMessageLevelNames.end()) {
        return it.value();
    }

	return "Unknown";
}

}
