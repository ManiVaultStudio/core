// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "GlobalSettingsGroupAction.h"

#include "actions/StringAction.h"
#include "actions/TriggerAction.h"

namespace mv
{

/**
 * Temporary files settings action class
 *
 * Action class which groups all temporary files global settings
 *
 * @author Thomas Kroes
 */
class TemporaryFilesSettingsAction final : public GlobalSettingsGroupAction
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     */
    TemporaryFilesSettingsAction(QObject* parent);

public: // Action getters

    gui::StringAction& getApplicationSessionIdAction() { return _applicationSessionIdAction; }
    gui::StringAction& getApplicationTemporaryDirAction() { return _applicationTemporaryDirAction; }
    gui::TriggerAction& getCleanManiVaultTemporaryDirAction() { return _cleanManiVaultTemporaryDirAction; }

private:
    gui::StringAction   _applicationSessionIdAction;        /** String action for the application session ID */
    gui::StringAction   _applicationTemporaryDirAction;     /** String action for the application temporary dir */
    gui::TriggerAction  _cleanManiVaultTemporaryDirAction;  /** Action which triggers the cleanup of the ManiVault temporary dir */
};

}
