// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractManager.h"

#include "actions/ScriptTriggerAction.h"

#include "util/Script.h"
#include "actions/ScriptTriggerAction.h"

#include <QObject>

namespace mv {

/**
 * Abstract scripting manager class
 *
 * Base abstract scripting manager class
 *
 * @ingroup mv_managers
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

    /**
     * Get script trigger actions for \p datasets
     * @param datasets Sequence of input datasets (order in which they were selected in the data hierarchy)
     * @return Vector of script trigger actions
     */
    virtual gui::ScriptTriggerActions getScriptTriggerActions(const Datasets& datasets) const = 0;
};

}
