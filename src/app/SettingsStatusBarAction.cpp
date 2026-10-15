// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SettingsStatusBarAction.h"

#include "CoreInterface.h"

#ifdef _DEBUG
#define SETTINGS_STATUS_BAR_ACTION_VERBOSE
#endif

using namespace mv;
using namespace mv::gui;

SettingsStatusBarAction::SettingsStatusBarAction(QObject* parent, const QString& title) :
    StatusBarAction(parent, title, "gears")
{
    setToolTip("Settings");

    addMenuAction(&settings().getEditSettingsAction());
}
