// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractTaskHandler.h"

#include "actions/GroupAction.h"

namespace hdps {

/**
 * Modal task handler class
 *
 * Interact with tasks in a modal tasks dialog.
 *
 * @author Thomas Kroes
 */
class ModalTaskHandler final : public AbstractTaskHandler
{
public:

    /**
    * Construct task handler with \p parent object
    * @param parent Pointer to parent object
    */
    ModalTaskHandler(QObject* parent);

    /** Initializes the handler */
    void init() override;

private:
    gui::GroupAction    _tasksGroupAction;      /** Groups modal tasks */
    
    /** Modal tasks dialog */
    static QDialog tasksDialog;
};

}
