// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ApplicationSettingsAction.h"
#include "Application.h"

#ifdef Q_OS_MACX
	#include "util/MacThemeHelper.h"
#endif

namespace mv::gui
{

ApplicationSettingsAction::ApplicationSettingsAction(QObject* parent) :
    GlobalSettingsGroupAction(parent, "Application"),
    _applicationSessionIdAction(this, "Application session ID", Application::current()->getId()),
    _appearanceOptionAction(this, "Appearance", QStringList({ "System", "Dark", "Light" }), "System"),
    _errorReportingConsentAction(this, "Consent..."),
    _allowErrorReportingAction(this, "Error reporting", false),
    _showErrorReportDialogAction(this, "Show error report dialog", true),
    _errorReportingAction(this, "Error reporting")
{
    _applicationSessionIdAction.setEnabled(false);

    addAction(&_applicationSessionIdAction);

    bool themesAvailable = false;

#ifdef Q_OS_MACX
    themesAvailable = macDarkThemeAvailable();
    // Temporary no option is provided, instead we fix for the light theme.
    // While changing the theme works, there is still a number of icons that do not work on dark theme.
    //macSetToLightTheme();
#endif // Q_OS_MACX
    
    if (themesAvailable)
    {
        _appearanceOptionAction.setDefaultWidgetFlags(gui::OptionAction::HorizontalButtons);
            
        addAction(&_appearanceOptionAction);
        
        const auto appearanceOptionCurrentIndexChanged = [this](const QString& currentText) -> void {
            if(currentText == "System")
            {
#ifdef Q_OS_MACX
                macSetToAutoTheme();
#endif // Q_OS_MACX
            }
            else if(currentText == "Dark")
            {
#ifdef Q_OS_MACX
                macSetToDarkTheme();
#endif // Q_OS_MACX
            }
            else if(currentText == "Light")
            {
#ifdef Q_OS_MACX
                macSetToLightTheme();
#endif // Q_OS_MACX
            }
        };

        connect(&_appearanceOptionAction, &gui::OptionAction::currentTextChanged, this, appearanceOptionCurrentIndexChanged);
    }

#ifdef _DEBUG
    _showErrorReportDialogAction.setChecked(false);
#endif

    _allowErrorReportingAction.setEnabled(false);

    addAction(&_errorReportingAction);

    _errorReportingAction.addAction(&_errorReportingConsentAction);
    _errorReportingAction.addAction(&_allowErrorReportingAction);
    _errorReportingAction.addAction(&_showErrorReportDialogAction);

    const auto allowErrorReportingChanged = [this]() -> void {
        _showErrorReportDialogAction.setEnabled(_allowErrorReportingAction.isChecked());
	};

    allowErrorReportingChanged();

    connect(&_allowErrorReportingAction, &ToggleAction::toggled, this, allowErrorReportingChanged);
}

}
