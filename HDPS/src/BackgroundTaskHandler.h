// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractTaskHandler.h"

namespace hdps {

/**
 * Background task handler class
 *
 * Interact with tasks in a main window status bar popup window.
 *
 * @author Thomas Kroes
 */
class BackgroundTaskHandler final : public AbstractTaskHandler
{
public:

    /**
    * Construct task handler with \p parent object
    * @param parent Pointer to parent object
    * @param task Pointer to task to handle
    */
    BackgroundTaskHandler(QObject* parent, Task* task);

    /** Initializes the handler */
    void init() override;
};

}
