// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractManager.h"

#include "actions/TriggerAction.h"

#include "ParametersSettingsAction.h"
#include "MiscellaneousSettingsAction.h"

namespace hdps
{

/**
 * Abstract settings manager
 *
 * Base abstract settings manager class for managing global settings
 *
 * @author Thomas Kroes
 */
class AbstractSettingsManager : public AbstractManager
{
    Q_OBJECT

public:

    /**
     * Construct settings manager with \p parent object
     * @param parent Pointer to parent object
     */
    AbstractSettingsManager(QObject* parent = nullptr) :
        AbstractManager(parent, "Settings")
    {
    }

    /** Opens the settings editor dialog */
    virtual void edit() = 0;

public: // Action getters

    virtual gui::TriggerAction& getEditSettingsAction() = 0;

public: // Global settings actions

    virtual ParametersSettingsAction& getParametersSettings() = 0;
    virtual MiscellaneousSettingsAction& getMiscellaneousSettings() = 0;
};

}
