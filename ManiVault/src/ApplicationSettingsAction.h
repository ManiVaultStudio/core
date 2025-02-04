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
    TriggerAction& getErrorLoggingConsentAction() { return _errorLoggingConsentAction; }
    ToggleAction& getAllowErrorLoggingAction() { return _allowErrorLoggingAction; }
    ToggleAction& getShowCrashReportDialogAction() { return _showCrashReportDialogAction; }
    VerticalGroupAction& getErrorLoggingSettingsAction() { return _errorLoggingSettingsAction; }
    StringAction& getErrorLoggingDsnAction() { return _errorLoggingDsnAction; }
    HorizontalGroupAction& getErrorLoggingAction() { return _errorLoggingAction; }
    
private:
    StringAction            _applicationSessionIdAction;    /** String action for the application session identifier */
    OptionAction            _appearanceOptionAction;        /** Options action for dark, light, or system appearance */
    TriggerAction           _errorLoggingConsentAction;     /** Shows the error logging consent dialog when triggered */
	ToggleAction            _allowErrorLoggingAction;       /** Toggle error logging on/off */         
    ToggleAction            _showCrashReportDialogAction;   /** Toggle crash dialog on/off */
    VerticalGroupAction     _errorLoggingSettingsAction;    /** Advanced error logging settings action */
    StringAction            _errorLoggingDsnAction;         /** Error logging data source name action */
    HorizontalGroupAction   _errorLoggingAction;            /** Groups error logging actions together */
};

}
