// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractManager.h"

#include <QObject>

namespace mv {

/**
 * Abstract scripting manager class
 *
 * Base abstract scripting manager class
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT AbstractScriptingManager : public AbstractManager
{
    Q_OBJECT

public:

    /**
     * Construct manager with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    AbstractScriptingManager(QObject* parent) :
        AbstractManager(parent, "Scripting")
    {
    }
};

}
