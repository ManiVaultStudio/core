// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "GlobalSettingsGroupAction.h"

#include "actions/StringAction.h"
#include "actions/ToggleAction.h"
#include "actions/OptionsAction.h"
#include "actions/TriggerAction.h"
#include "actions/HorizontalGroupAction.h"

namespace mv::gui
{

/**
 * Temporary directories settings action class
 *
 * Action class which groups all temporary directories global settings
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT TemporaryDirectoriesSettingsAction final : public GlobalSettingsGroupAction
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     */
    TemporaryDirectoriesSettingsAction(QObject* parent);

public: // Action getters

    StringAction& getApplicationTemporaryDirAction() { return _applicationTemporaryDirAction; }
    ToggleAction& getRemoveStaleTemporaryDirsAtStartupAction() { return _removeStaleTemporaryDirsAtStartupAction; }
    TriggerAction& getScanForStaleTemporaryDirectoriesAction() { return _scanForStaleTemporaryDirectoriesAction; }
    OptionsAction& getStaleTemporaryDirectoriesAction() { return _selectStaleTemporaryDirectoriesAction; }
    TriggerAction& getRemoveStaleTemporaryDirectoriesAction() { return _removeStaleTemporaryDirectoriesAction; }
    HorizontalGroupAction& getStaleTemporaryDirectoriesGroupAction() { return _staleTemporaryDirectoriesGroupAction; }

private:
    StringAction           _applicationTemporaryDirAction;             /** String action for the application temporary dir */
    ToggleAction           _removeStaleTemporaryDirsAtStartupAction;   /** Turn on/off the automatic removal of the application temporary directories at application startup */
    TriggerAction          _scanForStaleTemporaryDirectoriesAction;    /** Action which scans for stale temporary directories */
    OptionsAction          _selectStaleTemporaryDirectoriesAction;     /** Action which lists stale temporary directories */
    TriggerAction          _removeStaleTemporaryDirectoriesAction;     /** Action which triggers the cleanup of stale temporary directories */
    HorizontalGroupAction  _staleTemporaryDirectoriesGroupAction;      /** Group action for working with stale temporary directories */
};

}
