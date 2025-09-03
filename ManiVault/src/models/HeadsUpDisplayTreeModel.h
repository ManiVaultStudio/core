// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractHeadsUpDisplayModel.h"

namespace mv {

/**
 * Heads-up display tree model class
 *
 * Tree model for heads-up display items.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT HeadsUpDisplayTreeModel : public AbstractHeadsUpDisplayModel
{
public:

    /**
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    HeadsUpDisplayTreeModel(QObject* parent = nullptr);
};

}