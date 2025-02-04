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
    _errorLoggingConsentAction(this, "Consent..."),
    _allowErrorLoggingAction(this, "Error reporting", false),
    _showCrashReportDialogAction(this, "Show crash report dialog", true),
    _errorLoggingSettingsAction(this, "Settings"),
    _errorLoggingDsnAction(this, "Sentry DSN"),
    _errorLoggingAction(this, "Error reporting")
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
    _showCrashReportDialogAction.setChecked(false);
#endif

#ifdef ERROR_LOGGING
    _errorLoggingAction.setShowLabels(false);

    _allowErrorLoggingAction.setEnabled(false);
    _allowErrorLoggingAction.setToolTip("Toggle Sentry error logging");

    addAction(&_errorLoggingAction);

    _errorLoggingDsnAction.setSettingsPrefix(QString("%1/ErrorLogging/DSN").arg(getSettingsPrefix()));
    _errorLoggingDsnAction.setToolTip("The Sentry error logging data source name");
    _errorLoggingDsnAction.getValidator().setRegularExpression(QRegularExpression(R"(^https?://[a-f0-9]{32}@[a-z0-9\.-]+(:\d+)?/[\d]+$)"));

    _errorLoggingSettingsAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    _errorLoggingSettingsAction.setIconByName("cog");
    _errorLoggingSettingsAction.setPopupSizeHint({ 400, 0 });
    _errorLoggingSettingsAction.setLabelSizingType(LabelSizingType::Auto);
    _errorLoggingSettingsAction.setToolTip("Error logging settings");

    _errorLoggingSettingsAction.addAction(&_showCrashReportDialogAction);
    _errorLoggingSettingsAction.addAction(&_errorLoggingDsnAction);

    _errorLoggingAction.addAction(&_errorLoggingConsentAction);
    _errorLoggingAction.addAction(&_allowErrorLoggingAction);
    _errorLoggingAction.addAction(&_errorLoggingSettingsAction);

    _showCrashReportDialogAction.setSettingsPrefix(QString("%1/ErrorLogging/ShowCrashReportDialog").arg(getSettingsPrefix()));
    _showCrashReportDialogAction.setToolTip("Show report dialog when ManiVault Studio crashes");

    const auto allowErrorReportingChanged = [this]() -> void {
        _showCrashReportDialogAction.setEnabled(_allowErrorLoggingAction.isChecked());
	};

    allowErrorReportingChanged();

    connect(&_allowErrorLoggingAction, &ToggleAction::toggled, this, allowErrorReportingChanged);
#endif
}

}
