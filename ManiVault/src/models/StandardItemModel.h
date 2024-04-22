// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "NumberOfRowsAction.h"

#include <QStandardItemModel>

namespace mv
{

/**
 * Standard item model class
 *
 * Base MV standard item model class
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT StandardItemModel : public QStandardItemModel
{
    Q_OBJECT

public:

    /**
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    StandardItemModel(QObject* parent = nullptr);

public: // Action getters

    gui::NumberOfRowsAction& getNumberOfRowsAction() { return _numberOfRowsAction; }
    
    const gui::NumberOfRowsAction& getNumberOfRowsAction() const { return _numberOfRowsAction; }

private:
    gui::NumberOfRowsAction _numberOfRowsAction;       /** String action for displaying the number of rows */
};

}
