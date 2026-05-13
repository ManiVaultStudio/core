// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowPlan.h"

#include <QVariantMap>
#include <memory>

namespace mv::util
{

using sharedVariantMap = std::shared_ptr<QVariantMap>;

/** Result of an asynchronous toVariantMap() operation */
struct CORE_EXPORT AsyncToVariantMapResult
{
    WorkflowPlan        _workflowPlan;  /** Workflow plan associated with the asynchronous operation */
    sharedVariantMap    _variantMap;    /** Resulting variant map from the asynchronous operation */
};

/** Result of an asynchronous fromVariantMap() operation */
struct CORE_EXPORT AsyncFromVariantMapResult
{
    WorkflowPlan    _workflowPlan;  /** Workflow plan associated with the asynchronous operation */
};

}
