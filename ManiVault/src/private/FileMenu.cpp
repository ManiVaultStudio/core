// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "FileMenu.h"

#include <Application.h>
#include <CoreInterface.h>
#include <QOperatingSystemVersion>

#define TEST_STYLESHEET

using namespace mv;
using namespace mv::gui;

FileMenu::FileMenu(QWidget* parent /*= nullptr*/) :
    QMenu(parent),
    _exitApplictionAction(nullptr, "Exit")
{
    setTitle("File");
    setToolTip("File operations");
    
    //  Quit is by default in the app menu on macOS
    if (QOperatingSystemVersion::currentType() != QOperatingSystemVersion::MacOS) {
        
        _exitApplictionAction.setShortcut(QKeySequence("Alt+F4"));
        _exitApplictionAction.setShortcutContext(Qt::ApplicationShortcut);
        _exitApplictionAction.setIconByName("right-from-bracket");
        _exitApplictionAction.setToolTip("Exit ManiVault");
        
        connect(&_exitApplictionAction, &TriggerAction::triggered, this, []() -> void {
            Application::current()->quit();
        });
    }
    
    // macOS does not like populating the menu on show, so we rather do it explicitly here
    populate();
}

void FileMenu::showEvent(QShowEvent* event)
{
	QMenu::showEvent(event);

    if (QOperatingSystemVersion::currentType() == QOperatingSystemVersion::Windows)
        populate();
}

void FileMenu::populate()
{
    clear();

    addMenu(&projects().getNewProjectMenu());
    addAction(&projects().getOpenProjectAction());
    //addAction(&projects().getImportProjectAction());
    addAction(&projects().getSaveProjectAction());
    addAction(&projects().getSaveProjectAsAction());
    addAction(&projects().getEditProjectSettingsAction());
    addAction(&projects().getPublishAction());
    addMenu(projects().getRecentProjectsAction().getMenu(this));
    //addSeparator();
    //addMenu(Application::core()->getWorkspaceManager().getMenu());
    addSeparator();
    addMenu(&projects().getImportDataMenu());
    addSeparator();
    addAction(&projects().getPluginManagerAction());
    addSeparator();
    addAction(&settings().getEditSettingsAction());
    addSeparator();
    addAction(&projects().getShowStartPageAction());
    addAction(&help().getShowLearningCenterPageAction());
    
    //  Quit is by default in the app menu on macOS
    //if(QOperatingSystemVersion::currentType() != QOperatingSystemVersion::MacOS) {
    //addSeparator();
    //addAction(&_exitApplictionAction);
    //}

#if defined(_DEBUG) && defined(TEST_STYLESHEET)
    /*
    addSeparator();
    
    auto applyStyleSheetAction = new TriggerAction(this, "Apply Stylesheet");

    connect(applyStyleSheetAction, &TriggerAction::triggered, &workspaces(), &AbstractWorkspaceManager::applyStyleSheet);

    addAction(applyStyleSheetAction);
    */
#endif
}
