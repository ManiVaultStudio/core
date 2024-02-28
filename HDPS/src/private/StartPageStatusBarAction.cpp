// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StartPageStatusBarAction.h"

#ifdef _DEBUG
    #define START_PAGE_STATUS_BAR_ACTION_VERBOSE
#endif

using namespace mv;
using namespace mv::gui;

StartPageStatusBarAction::StartPageStatusBarAction(QObject* parent, const QString& title) :
    StatusBarAction(parent, title, "door-open")
{
    const auto updateTooltip = [this]() -> void {
        setToolTip(projects().getShowStartPageAction().isChecked() ? "Go back to the project" : "Show the start page");
    };

    updateTooltip();

    connect(&projects().getShowStartPageAction(), &ToggleAction::toggled, this, updateTooltip);

    connect(this, &StatusBarAction::toolButtonClicked, this, []() -> void {
        projects().getShowStartPageAction().toggle();
    });
}
