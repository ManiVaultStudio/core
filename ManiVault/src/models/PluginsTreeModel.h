// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractPluginsModel.h"

namespace mv {

/**
 * Plugins tree model class
 *
 * Tree model for loaded plugins
 *
 * @author Thomas Kroes
 */
class PluginsTreeModel : public AbstractPluginsModel
{
public:

    /**
     * Construct plugin manager model with \p parent
     * @param parent Pointer to parent object
     */
    PluginsTreeModel(QObject* parent = nullptr);
};

}