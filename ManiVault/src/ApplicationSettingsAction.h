// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "GlobalSettingsGroupAction.h"

#include "actions/StringAction.h"
#include "actions/OptionAction.h"
#include "actions/TriggerAction.h"
#include "actions/HorizontalGroupAction.h"

namespace mv::gui
{

/**
 * Application global settings action class
 *
 * Groups all global settings for the application
 *
 * @author Thomas Kroes
 * @author Thomas Höllt
 */
class CORE_EXPORT ApplicationSettingsAction final : public GlobalSettingsGroupAction
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     */
    ApplicationSettingsAction(QObject* parent);

public: // Action getters

    StringAction& getApplicationSessionIdAction() { return _applicationSessionIdAction; }
    OptionAction& getAppearanceOptionAction() { return _appearanceOptionAction; }
    TriggerAction& getErrorReportingConsentAction() { return _errorReportingConsentAction; }
    ToggleAction& getAllowErrorReportingAction() { return _allowErrorReportingAction; }
    ToggleAction& getShowErrorReportDialogAction() { return _showErrorReportDialogAction; }
    HorizontalGroupAction& getErrorReportingAction() { return _errorReportingAction; }
    
private:
    StringAction            _applicationSessionIdAction;    /** String action for the application session identifier */
    OptionAction            _appearanceOptionAction;        /** Options action for dark, light, or system appearance */
    TriggerAction           _errorReportingConsentAction;   /** Shows the error reporting consent dialog when triggered */
	ToggleAction            _allowErrorReportingAction;     /** Toggle error reporting on/off */         
    ToggleAction            _showErrorReportDialogAction;   /** Toggle error report dialog on/off */
    HorizontalGroupAction   _errorReportingAction;          /** Groups error reporting actions together */
};

}
