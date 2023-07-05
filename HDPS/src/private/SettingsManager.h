// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractSettingsManager.h"

namespace hdps
{

class SettingsManager final : public AbstractSettingsManager
{
    Q_OBJECT

public:

    /** Default constructor */
    SettingsManager();

    /** Perform manager startup initialization */
    void initialize() override;

    /** Resets the contents of the actions manager */
    void reset() override;

    /** Opens the settings editor dialog */
    void edit() override;

public: // Action getters

    gui::TriggerAction& getEditSettingsAction() override { return _editSettingsAction; }

public: // Global settings actions

    ParametersSettingsAction& getParametersSettings() override { return _parametersSettingsAction; };
    MiscellaneousSettingsAction& getMiscellaneousSettings() override { return _miscellaneousSettingsAction; };

private:
    gui::TriggerAction          _editSettingsAction;            /** Action for triggering the settings dialog */
    ParametersSettingsAction    _parametersSettingsAction;      /** Parameters global settings */
    MiscellaneousSettingsAction _miscellaneousSettingsAction;   /** Miscellaneous global settings */
};

}
