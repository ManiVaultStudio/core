// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "AppearanceSettingsAction.h"
#include "Application.h"

namespace mv::gui
{

AppearanceSettingsAction::AppearanceSettingsAction(QObject* parent) :
    GlobalSettingsGroupAction(parent, "Appearance"),
    _applicationSessionIdAction(this, "Application session ID", Application::current()->getId()),
    _lightDarkSystemColorSchemeAction(this, "Color scheme")
{
    _applicationSessionIdAction.setEnabled(false);

#ifdef _DEBUG
    addAction(&_applicationSessionIdAction);
#endif

    _lightDarkSystemColorSchemeAction.setShowLabels(false);

    connect(Application::current(), &Application::coreInitialized, this, [this](CoreInterface* core) -> void {
        addAction(&mv::theme().getColorSchemeModeAction());

        _lightDarkSystemColorSchemeAction.addAction(&mv::theme().getSystemLightColorSchemeAction());
        _lightDarkSystemColorSchemeAction.addAction(&mv::theme().getSystemDarkColorSchemeAction());

        addAction(&_lightDarkSystemColorSchemeAction, -1, WidgetConfigurationFunction());

#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
        addAction(&mv::theme().getCustomColorSchemeAction());
#endif
    });
}

}
