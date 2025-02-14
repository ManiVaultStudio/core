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
    _themeGroupAction(this, "Theme"),
    _appearanceOptionAction(this, "Appearance", QStringList({ "System", "Dark", "Light" }), "System")
{
    _applicationSessionIdAction.setEnabled(false);

    addAction(&_applicationSessionIdAction);

    _themeGroupAction.setShowLabels(false);

    _appearanceOptionAction.setDefaultWidgetFlags(gui::OptionAction::HorizontalButtons);

    bool themesAvailable = false;

#ifdef Q_OS_MACX
    themesAvailable = macDarkThemeAvailable();
    // Temporary no option is provided, instead we fix for the light theme.
    // While changing the theme works, there is still a number of icons that do not work on dark theme.
    //macSetToLightTheme();
#endif // Q_OS_MACX

#ifdef Q_OS_MACX
    if (themesAvailable)
    {
        addAction(&_appearanceOptionAction);
        
        const auto appearanceOptionCurrentIndexChanged = [this](const QString& currentText) -> void {
            if (currentText == "System") {
                macSetToAutoTheme();
            } else if(currentText == "Dark") {
                macSetToDarkTheme();
            } else if(currentText == "Light") {
                macSetToLightTheme();
            }
        };

        connect(&_appearanceOptionAction, &gui::OptionAction::currentTextChanged, this, appearanceOptionCurrentIndexChanged);
    }
#endif

#ifdef WIN32
	#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)

	    connect(Application::current(), &Application::coreInitialized, this, [this](CoreInterface* core) -> void {
	        addAction(&_themeGroupAction);

	        _themeGroupAction.addAction(&mv::theme().getUseSystemThemeAction());
	        _themeGroupAction.addAction(&mv::theme().getLightThemeAction());
	        _themeGroupAction.addAction(&mv::theme().getDarkThemeAction());
	        _themeGroupAction.addAction(&mv::theme().getCustomThemeAction());
        });

	#else
		hide();
	#endif
#endif
	
}

}
