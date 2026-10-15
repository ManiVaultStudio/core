// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractScriptsModel.h"

namespace mv {

/**
 * Scripts tree model class
 *
 * Tree model for Scripts
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ScriptsTreeModel : public AbstractScriptsModel
{
public:

    /**
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    ScriptsTreeModel(QObject* parent = nullptr);
};

}