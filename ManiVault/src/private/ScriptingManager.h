// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <AbstractScriptingManager.h>

namespace mv
{

class ScriptingManager : public AbstractScriptingManager
{
    Q_OBJECT

public:

    /**
     * Construct manager with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    ScriptingManager(QObject* parent);

    /** Reset when destructed */
    ~ScriptingManager() override;

    /** Perform event manager startup initialization */
    void initialize() override;

    /** Resets the contents of the event manager */
    void reset() override;

    /**
     * Get script trigger actions for \p datasets
     * @param datasets Sequence of input datasets (order in which they were selected in the data hierarchy)
     * @return Vector of script trigger actions
     */
    gui::ScriptTriggerActions getScriptTriggerActions(const Datasets& datasets) const override;
};

}
