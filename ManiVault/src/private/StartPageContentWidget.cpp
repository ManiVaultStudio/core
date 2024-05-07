// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StartPageContentWidget.h"

#include "StartPageAction.h"
#include "StartPageWidget.h"

#include <QDebug>

using namespace mv;
using namespace mv::gui;

StartPageContentWidget::StartPageContentWidget(QWidget* parent /*= nullptr*/) :
    PageContentWidget(parent),
    _toolbarLayout(),
    _compactViewAction(this, "Compact"),
    _toggleOpenCreateProjectAction(this, "Open & Create", true),
    _toggleRecentProjectsAction(this, "Recent Projects", true),
    _toggleExampleProjectsAction(this, "Examples"),
    _toggleProjectFromWorkspaceAction(this, "Project From Workspace"),
    _toggleProjectFromDataAction(this, "Project From Data", true),
    _toggleTutorialVideosAction(this, "Instructional Videos"),
    _settingsAction(this, "Settings"),
    _openProjectWidget(this),
    _getStartedWidget(this)
{
    setObjectName("StartPageContentWidget");

    _compactViewAction.setSettingsPrefix("StartPage/ToggleCompactView");
    _toggleOpenCreateProjectAction.setSettingsPrefix("StartPage/ToggleOpenCreateProject");
    _toggleRecentProjectsAction.setSettingsPrefix("StartPage/ToggleRecentProjects");
    _toggleExampleProjectsAction.setSettingsPrefix("StartPage/ToggleExampleProjects");
    _toggleProjectFromWorkspaceAction.setSettingsPrefix("StartPage/ToggleProjectFromWorkspace");
    _toggleProjectFromDataAction.setSettingsPrefix("StartPage/ToggleProjectFromData");
    _toggleTutorialVideosAction.setSettingsPrefix("StartPage/ToggleTutorialVideos");

    _settingsAction.setText("Toggle Views");
    
    _settingsAction.setIconByName("eye");

    _settingsAction.addAction(&_toggleOpenCreateProjectAction);
    _settingsAction.addAction(&_toggleRecentProjectsAction);
    _settingsAction.addAction(&_toggleExampleProjectsAction);
    _settingsAction.addAction(&_toggleProjectFromWorkspaceAction);
    _settingsAction.addAction(&_toggleProjectFromDataAction);
    _settingsAction.addAction(&_toggleTutorialVideosAction);

    _toolbarLayout.setContentsMargins(35, 10, 35, 10);

    getColumnsLayout().addWidget(&_openProjectWidget);
    getColumnsLayout().addWidget(&_getStartedWidget);

    getMainLayout().addLayout(&_toolbarLayout);

    _toolbarLayout.addWidget(_compactViewAction.createWidget(this));
    _toolbarLayout.addStretch(1);
    _toolbarLayout.addWidget(_settingsAction.createCollapsedWidget(this));

    connect(&_compactViewAction, &ToggleAction::toggled, this, &StartPageContentWidget::updateActions);
}

void StartPageContentWidget::updateActions()
{
    StartPageAction::setCompactView(_compactViewAction.isChecked());

    _openProjectWidget.updateActions();
    _getStartedWidget.updateActions();
}
