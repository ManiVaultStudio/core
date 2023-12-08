// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "GlobalSettingsGroupAction.h"

#include "actions/StringAction.h"
#include "actions/OptionsAction.h"
#include "actions/TriggerAction.h"
#include "actions/HorizontalGroupAction.h"

namespace mv
{

/**
 * Temporary directories settings action class
 *
 * Action class which groups all temporary directories global settings
 *
 * @author Thomas Kroes
 */
class TemporaryDirectoriesSettingsAction final : public GlobalSettingsGroupAction
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     */
    TemporaryDirectoriesSettingsAction(QObject* parent);

public: // Action getters

    gui::StringAction& getApplicationTemporaryDirAction() { return _applicationTemporaryDirAction; }
    gui::TriggerAction& getRescanForStaleTemporaryDirectoriesAction() { return _rescanForStaleTemporaryDirectoriesAction; }
    gui::OptionsAction& getStaleTemporaryDirectoriesAction() { return _selectStaleTemporaryDirectoriesAction; }
    gui::TriggerAction& getRemoveStaleTemporaryDirectoriesAction() { return _removeStaleTemporaryDirectoriesAction; }
    gui::HorizontalGroupAction& getStaleTemporaryDirectoriesGroupAction() { return _staleTemporaryDirectoriesGroupAction; }

private:
    gui::StringAction           _applicationTemporaryDirAction;             /** String action for the application temporary dir */
    gui::TriggerAction          _rescanForStaleTemporaryDirectoriesAction;  /** Action which re-scans for stale temporary directories */
    gui::OptionsAction          _selectStaleTemporaryDirectoriesAction;           /** Action which lists stale temporary directories */
    gui::TriggerAction          _removeStaleTemporaryDirectoriesAction;     /** Action which triggers the cleanup of stale temporary directories */
    gui::HorizontalGroupAction  _staleTemporaryDirectoriesGroupAction;      /** Group action for working with stale temporary directories */
};

}
