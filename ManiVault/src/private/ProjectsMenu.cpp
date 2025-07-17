// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectsMenu.h"

#include "CoreInterface.h"

#include <util/Miscellaneous.h>
#include <actions/TriggerAction.h>

#include <QOperatingSystemVersion>

using namespace mv;
using namespace mv::util;
using namespace mv::gui;
using namespace mv::plugin;

ProjectsMenu::ProjectsMenu(QWidget* parent /*= nullptr*/) :
    QMenu(parent)
{
    setTitle("Projects");
    setToolTip("Projects");

    populate();
}

void ProjectsMenu::showEvent(QShowEvent* event)
{
    QMenu::showEvent(event);

    if (QOperatingSystemVersion::currentType() == QOperatingSystemVersion::Windows)
        populate();
}

void ProjectsMenu::populate()
{
    clear();
    
    addAction(&mv::help().getShowLearningCenterPageAction());
    
}
