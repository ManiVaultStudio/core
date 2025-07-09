// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "SortFilterProxyModel.h"

namespace mv {

/**
 * Scripts filter model class
 *
 * Sorting and filtering model for scripts
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ScriptsFilterModel : public SortFilterProxyModel
{
public:

    /** 
     * Construct the filter model with \p parent
     * @param parent Pointer to parent object
    */
    ScriptsFilterModel(QObject* parent = nullptr);
};

}