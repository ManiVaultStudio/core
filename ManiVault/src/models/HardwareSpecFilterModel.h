// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "models/SortFilterProxyModel.h"

namespace mv {

class AbstractHardwareSpecModel;

/**
 * Hardware specs filter model class
 *
 * Sorting and filtering model for the hardware specs model.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT HardwareSpecFilterModel : public SortFilterProxyModel
{
public:

    /** 
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
    */
    HardwareSpecFilterModel(QObject* parent = nullptr);

    /**
     * Returns whether \p row with \p parent is filtered out (false) or in (true)
     * @param row Row index
     * @param parent Parent index
     * @return Boolean indicating whether the item is filtered in or out
     */
    bool filterAcceptsRow(int row, const QModelIndex& parent) const override;
};

}
