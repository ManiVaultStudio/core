// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractPluginFactoriesModel.h"

namespace mv {

/**
 * Plugin factories tree model class
 *
 * tree model for loaded plugin factories
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT PluginFactoriesTreeModel : public AbstractPluginFactoriesModel
{
public:

    /**
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    PluginFactoriesTreeModel(QObject* parent = nullptr);
};

}