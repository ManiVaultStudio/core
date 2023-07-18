// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractManager.h"

#ifdef _DEBUG
    #define ABSTRACT_PROGRESS_MANAGER_VERBOSE
#endif

namespace hdps
{

/**
 * Abstract progress manager class
 *
 * Base abstract progress manager class for global progress management.
 *
 * @author Thomas Kroes
 */
class AbstractProgressManager : public AbstractManager
{
    Q_OBJECT

public:

    /**
     * Construct progress manager with \p parent object
     * @param parent Pointer to parent object
     */
    AbstractProgressManager(QObject* parent = nullptr) :
        AbstractManager(parent, "Progress")
    {
    }

signals:
};

}
