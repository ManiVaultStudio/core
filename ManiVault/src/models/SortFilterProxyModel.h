// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "NumberOfRowsAction.h"

#include <QSortFilterProxyModel>

namespace mv
{

class DatasetImpl;

/**
 * Filter model class
 *
 * Base MV filter model class
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT SortFilterProxyModel : public QSortFilterProxyModel
{
public:

    /** Construct with parent \p parent object
     * @param parent Pointer to parent object
    */
    SortFilterProxyModel(QObject* parent = nullptr);

public: // Action getters

    gui::NumberOfRowsAction& getNumberOfRowsAction() { return _numberOfRowsAction; }

private:
    gui::NumberOfRowsAction _numberOfRowsAction;       /** String action for displaying the number of rows */
    
};

}
