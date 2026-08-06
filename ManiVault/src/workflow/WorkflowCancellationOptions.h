// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

namespace mv::workflow
{

/** Options that configure cooperative workflow cancellation. */
struct CORE_EXPORT WorkflowCancellationOptions
{
    bool enabled = false;     /**< Whether workflow progress tasks expose user cancellation */
};

}
